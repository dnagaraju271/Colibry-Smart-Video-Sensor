#!/usr/bin/env python3
"""Inspect a YOLO candidate and report Colibry input/operator risks.

This tool never flashes hardware and never invokes the compiler unless
--run-compiler is supplied. Compiler commands are intentionally configured
outside this source file because the nnpa-compiler CLI is SDK-version-specific.
"""
from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from collections import Counter
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CONFIG = ROOT / "configs" / "yolo_colibry_check.json"
SUPPORTED_SUFFIXES = {".onnx", ".tflite", ".pt", ".pth", ".h5", ".keras"}
DEFAULT_PATTERNS = [f"*yolo*{suffix}" for suffix in sorted(SUPPORTED_SUFFIXES)]


def expand_path(value: str | Path, root: Path = ROOT) -> Path:
    """Expand ~ and environment variables; resolve relative paths from repo root."""
    expanded = Path(os.path.expandvars(os.path.expanduser(str(value))))
    return (expanded if expanded.is_absolute() else root / expanded).resolve()


def load_config(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"Configuration file not found: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"Invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise ValueError("Configuration root must be a JSON object.")
    data.setdefault("model_path", "")
    data.setdefault("search_roots", ["models", "hardware/artifacts", "~/person_detector_candidate/model"])
    data.setdefault("filename_patterns", DEFAULT_PATTERNS)
    data.setdefault("target", {})
    data.setdefault("compiler", {})
    target_defaults = {
        "input_width": 160,
        "input_height": 128,
        "input_channels": 1,
        "input_color": "grayscale",
        "preferred_format": "tflite-int8",
    }
    for key, value in target_defaults.items():
        data["target"].setdefault(key, value)
    data["compiler"].setdefault("path", "~/venvs/colibry-compiler/bin/nnpa-compiler")
    data["compiler"].setdefault("command", [])
    data["compiler"].setdefault("output_dir", "build/yolo_colibry")
    data["compiler"].setdefault("timeout_seconds", 300)
    return data


def discover_candidates(config: dict[str, Any], extra_roots: list[str] | None = None) -> list[Path]:
    patterns = [str(p).lower() for p in config.get("filename_patterns", DEFAULT_PATTERNS)]
    roots = list(config.get("search_roots", [])) + list(extra_roots or [])
    found: set[Path] = set()
    for raw_root in roots:
        directory = expand_path(raw_root)
        if not directory.is_dir():
            continue
        for candidate in directory.rglob("*"):
            if not candidate.is_file() or candidate.suffix.lower() not in SUPPORTED_SUFFIXES:
                continue
            name = candidate.name.lower()
            if any(_simple_pattern_match(name, pattern) for pattern in patterns):
                found.add(candidate.resolve())
    return sorted(found, key=lambda p: (p.name.lower(), str(p).lower()))


def _simple_pattern_match(name: str, pattern: str) -> bool:
    """Small case-insensitive glob matcher for filenames only."""
    import fnmatch
    return fnmatch.fnmatch(name.lower(), pattern.lower())


def _shape_values(value: Any) -> list[Any] | None:
    if value is None:
        return None
    try:
        shape = list(value)
    except TypeError:
        return None
    result: list[Any] = []
    for dim in shape:
        if hasattr(dim, "dim_value"):
            if dim.HasField("dim_value") and dim.dim_value > 0:
                result.append(int(dim.dim_value))
            elif dim.HasField("dim_param") and dim.dim_param:
                result.append(str(dim.dim_param))
            else:
                result.append("?")
        elif hasattr(dim, "value"):
            result.append(int(dim.value) if dim.value is not None else "?")
        else:
            try:
                n = int(dim)
                result.append(n if n > 0 else "?")
            except (TypeError, ValueError):
                result.append(str(dim) if dim is not None else "?")
    return result


def _tensor_details(name: str, shape: Any, layout: str = "unknown", **extra: Any) -> dict[str, Any]:
    result = {"name": name, "shape": _shape_values(shape), "layout": layout}
    result.update(extra)
    return result


def inspect_onnx(path: Path) -> dict[str, Any]:
    try:
        import onnx  # type: ignore
    except ImportError:
        return {"inspected": False, "reason": "Optional package 'onnx' is not installed."}
    try:
        model = onnx.load(str(path), load_external_data=False)
        onnx.checker.check_model(model)
        initializers = {x.name for x in model.graph.initializer}
        inputs = [
            _tensor_details(item.name, item.type.tensor_type.shape.dim)
            for item in model.graph.input if item.name not in initializers
        ]
        outputs = [
            _tensor_details(item.name, item.type.tensor_type.shape.dim)
            for item in model.graph.output
        ]
        ops = Counter(node.op_type for node in model.graph.node)
        opset = sorted({int(item.version) for item in model.opset_import if item.domain in ("", "ai.onnx")})
        return {
            "inspected": True,
            "format": "ONNX",
            "checker": "passed",
            "opsets": opset,
            "inputs": inputs,
            "outputs": outputs,
            "operator_count": sum(ops.values()),
            "operators": dict(sorted(ops.items())),
            "notes": ["ONNX validation passed; Colibry NNPA operator support is not proven by ONNX validation."],
        }
    except Exception as exc:  # report parser/checker errors instead of crashing the workflow
        return {"inspected": False, "format": "ONNX", "reason": f"{type(exc).__name__}: {exc}"}


def _get_tflite_interpreter() -> tuple[Any | None, str | None]:
    try:
        from tflite_runtime.interpreter import Interpreter  # type: ignore
        return Interpreter, "tflite_runtime"
    except ImportError:
        pass
    try:
        import tensorflow as tf  # type: ignore
        return tf.lite.Interpreter, f"TensorFlow {getattr(tf, '__version__', 'unknown')}"
    except ImportError:
        return None, None


def inspect_tflite(path: Path) -> dict[str, Any]:
    interpreter_cls, source = _get_tflite_interpreter()
    if interpreter_cls is None:
        return {"inspected": False, "reason": "Install TensorFlow or tflite-runtime to inspect TFLite tensors/operators."}
    try:
        interpreter = interpreter_cls(model_path=str(path))
        interpreter.allocate_tensors()
        inputs, outputs = [], []
        for item in interpreter.get_input_details():
            quant = item.get("quantization_parameters", {})
            inputs.append(_tensor_details(
                item.get("name", "input"), item.get("shape_signature", item.get("shape")),
                layout="NHWC" if len(item.get("shape", [])) == 4 else "unknown",
                dtype=str(item.get("dtype", "")),
                quantization=item.get("quantization"),
                quantization_scales=list(quant.get("scales", [])),
                quantization_zero_points=list(quant.get("zero_points", [])),
            ))
        for item in interpreter.get_output_details():
            quant = item.get("quantization_parameters", {})
            outputs.append(_tensor_details(
                item.get("name", "output"), item.get("shape_signature", item.get("shape")),
                layout="NHWC" if len(item.get("shape", [])) == 4 else "unknown",
                dtype=str(item.get("dtype", "")),
                quantization=item.get("quantization"),
                quantization_scales=list(quant.get("scales", [])),
                quantization_zero_points=list(quant.get("zero_points", [])),
            ))
        ops = Counter(item.get("op_name", "UNKNOWN") for item in interpreter._get_ops_details())
        return {
            "inspected": True, "format": "TFLite", "inspector": source,
            "inputs": inputs, "outputs": outputs,
            "operator_count": sum(ops.values()), "operators": dict(sorted(ops.items())),
            "notes": ["TFLite inspection does not establish support in this Colibry compiler version."],
        }
    except Exception as exc:
        return {"inspected": False, "format": "TFLite", "reason": f"{type(exc).__name__}: {exc}"}


def inspect_ultralytics(path: Path) -> dict[str, Any]:
    try:
        from ultralytics import YOLO  # type: ignore
    except ImportError:
        return {"inspected": False, "reason": "Optional package 'ultralytics' is not installed."}
    try:
        wrapped = YOLO(str(path), task="detect")
        core = getattr(wrapped, "model", None)
        module_list = getattr(core, "model", [])
        layer_names = [type(layer).__name__ for layer in module_list]
        stride = getattr(core, "stride", None)
        stride_values = stride.tolist() if hasattr(stride, "tolist") else (list(stride) if stride is not None else None)
        names = getattr(wrapped, "names", None)
        if isinstance(names, dict):
            names = {str(k): str(v) for k, v in names.items()}
        return {
            "inspected": True, "format": "Ultralytics/PyTorch", "task": getattr(wrapped, "task", "unknown"),
            "inputs": [{"name": "YOLO inference input", "shape": None, "layout": "usually NCHW",
                        "note": "Weights do not necessarily encode a fixed input resolution; export settings matter."}],
            "outputs": [],
            "layer_count": len(layer_names), "layers": layer_names,
            "stride": stride_values, "class_names": names,
            "notes": ["PyTorch/Ultralytics load succeeded. A compiled static graph and operator support still need checking."],
        }
    except Exception as exc:
        return {"inspected": False, "format": "Ultralytics/PyTorch", "reason": f"{type(exc).__name__}: {exc}"}


def inspect_keras(path: Path) -> dict[str, Any]:
    try:
        import tensorflow as tf  # type: ignore
    except ImportError:
        return {"inspected": False, "reason": "TensorFlow is not installed to inspect this Keras/H5 model."}
    try:
        model = tf.keras.models.load_model(str(path), compile=False)
        inputs = []
        for i, tensor in enumerate(model.inputs):
            inputs.append(_tensor_details(getattr(tensor, "name", f"input_{i}"), tensor.shape))
        outputs = []
        for i, tensor in enumerate(model.outputs):
            outputs.append(_tensor_details(getattr(tensor, "name", f"output_{i}"), tensor.shape))
        ops = Counter(type(layer).__name__ for layer in model.layers)
        return {
            "inspected": True, "format": path.suffix.lower().lstrip(".").upper(),
            "inputs": inputs, "outputs": outputs, "layer_count": len(model.layers),
            "operators": dict(sorted(ops.items())),
            "notes": ["Keras inspection is informational; convert/export to a compiler-supported form before NNPA validation."],
        }
    except Exception as exc:
        return {"inspected": False, "format": path.suffix.lower().lstrip(".").upper(),
                "reason": f"{type(exc).__name__}: {exc}"}


def inspect_model(path: Path) -> dict[str, Any]:
    suffix = path.suffix.lower()
    if suffix == ".onnx":
        return inspect_onnx(path)
    if suffix == ".tflite":
        return inspect_tflite(path)
    if suffix in {".pt", ".pth"}:
        return inspect_ultralytics(path)
    if suffix in {".h5", ".keras"}:
        return inspect_keras(path)
    return {"inspected": False, "reason": f"Unsupported extension: {suffix}"}


def assess_inputs(model_info: dict[str, Any], target: dict[str, Any]) -> list[str]:
    warnings: list[str] = []
    inputs = model_info.get("inputs") or []
    if not inputs:
        warnings.append("Input tensor metadata was not available; configure/export a fixed input before compiler testing.")
        return warnings
    for item in inputs:
        shape = item.get("shape")
        if not shape or len(shape) != 4:
            warnings.append(f"Input '{item.get('name', 'unnamed')}' is not a known rank-4 image tensor: {shape}.")
            continue
        if not isinstance(shape[0], int):
            warnings.append(
                f"Input '{item.get('name', 'unnamed')}' batch dimension is dynamic/unknown; "
                "a static batch size may be required for compilation."
            )
        # Determine standard NCHW/NHWC layouts from the channel dimension.
        layout = item.get("layout", "unknown")
        channels = height = width = None
        if layout == "NHWC":
            _, height, width, channels = shape
        elif layout == "NCHW":
            _, channels, height, width = shape
        else:
            second = shape[1]
            last = shape[3]
            if second in (1, 3, 4):
                layout, channels, height, width = "NCHW", second, shape[2], shape[3]
            elif last in (1, 3, 4):
                layout, height, width, channels = "NHWC", shape[1], shape[2], last
        item["layout"] = layout
        if channels is not None and isinstance(channels, int) and channels != int(target["input_channels"]):
            warnings.append(
                f"Input '{item.get('name', 'unnamed')}' has {channels} channels; "
                f"configured target requires {target['input_channels']} ({target.get('input_color', 'unspecified')})."
            )
        for label, actual, expected in (
            ("height", height, int(target["input_height"])),
            ("width", width, int(target["input_width"])),
        ):
            if isinstance(actual, int) and actual != expected:
                warnings.append(f"Input '{item.get('name', 'unnamed')}' {label} is {actual}; target is {expected}.")
            elif not isinstance(actual, int):
                warnings.append(f"Input '{item.get('name', 'unnamed')}' {label} is dynamic/unknown; compile-time size may be required.")
    return warnings


def check_compiler(config: dict[str, Any]) -> dict[str, Any]:
    compiler_value = str(config["compiler"].get("path", "nnpa-compiler"))
    candidate = expand_path(compiler_value) if ("/" in compiler_value or compiler_value.startswith("~")) else None
    executable = str(candidate) if candidate is not None else shutil.which(compiler_value)
    if not executable or not Path(executable).is_file():
        return {"found": False, "path": executable or compiler_value,
                "note": "Compiler executable not found; this is a path check only."}
    return {"found": True, "path": executable, "note": "Executable exists; no compilation was run."}


def run_compiler(config: dict[str, Any], model: Path) -> dict[str, Any]:
    compiler_cfg = config["compiler"]
    compiler_value = str(compiler_cfg.get("path", "nnpa-compiler"))
    compiler_path = expand_path(compiler_value) if ("/" in compiler_value or compiler_value.startswith("~")) else Path(shutil.which(compiler_value) or compiler_value)
    if model.suffix.lower() in {".pt", ".pth"}:
        raise ValueError(
            "The installed nnpa-compiler CLI accepts TensorFlow, ONNX, and TFLite models, "
            "not raw .pt/.pth weights. Export the YOLO checkpoint to ONNX first."
        )
    command_template = compiler_cfg.get("command") or ["{compiler}", "{model}", "--verbose"]
    if not isinstance(command_template, list) or not command_template:
        raise ValueError("compiler.command must be a non-empty list in the JSON config.")
    output_dir = expand_path(compiler_cfg.get("output_dir", "~/models/colibry_yolo/compiler_test"))
    output_dir.mkdir(parents=True, exist_ok=True)
    output = output_dir / f"{model.stem}.nnpa"
    replacements = {
        "{compiler}": str(compiler_path),
        "{model}": str(model.resolve()),
        "{output}": str(output),
        "{root}": str(ROOT),
    }
    command: list[str] = []
    for token in command_template:
        rendered = str(token)
        for marker, value in replacements.items():
            rendered = rendered.replace(marker, value)
        if rendered.startswith("~"):
            rendered = str(expand_path(rendered))
        command.append(rendered)
    print("Compiler command:", json.dumps(command))
    before = {path.resolve() for path in output_dir.glob("*.nnpa")}
    result = subprocess.run(
        command, cwd=output_dir, text=True, capture_output=True,
        timeout=int(compiler_cfg.get("timeout_seconds", 300)), check=False,
    )
    log_path = output_dir / f"{model.stem}_compiler.log"
    log_path.write_text(
        "COMMAND: " + json.dumps(command) + "\n"
        + f"RETURN CODE: {result.returncode}\n\nSTDOUT:\n" + result.stdout
        + "\nSTDERR:\n" + result.stderr,
        encoding="utf-8",
    )
    after = {path.resolve() for path in output_dir.glob("*.nnpa")}
    created = sorted(str(path) for path in after - before)
    combined = result.stdout + "\n" + result.stderr
    import re
    unsupported = re.search(r"(?:layer|operator)\s+([A-Za-z0-9_]+)\s+is unsupported", combined, re.IGNORECASE)
    return {
        "command": command,
        "working_directory": str(output_dir),
        "return_code": result.returncode,
        "stdout": result.stdout[-12000:],
        "stderr": result.stderr[-12000:],
        "log_file": str(log_path),
        "expected_output": str(output),
        "output_exists": output.is_file(),
        "created_nnpa_files": created,
        "first_unsupported_operator": unsupported.group(1) if unsupported else None,
    }


def print_report(report: dict[str, Any]) -> None:
    print("\n=== Colibry YOLO Compatibility Check ===")
    print(f"Model: {report.get('model', 'not selected')}")
    print(f"Format: {report.get('inspection', {}).get('format', 'unknown')}")
    print(f"File size: {report.get('file_size_bytes', 'n/a')} bytes")
    inspection = report.get("inspection", {})
    print(f"Structural inspection: {'PASS' if inspection.get('inspected') else 'NOT AVAILABLE'}")
    if inspection.get("reason"):
        print("Reason:", inspection["reason"])
    target = report.get("target", {})
    print(f"Colibry target: {target.get('input_width')}x{target.get('input_height')}x{target.get('input_channels')} "
          f"{target.get('input_color', '')}; preferred format={target.get('preferred_format')}")
    for direction in ("inputs", "outputs"):
        for item in inspection.get(direction, []) or []:
            print(f"{direction[:-1].capitalize()}: {item.get('name')} shape={item.get('shape')} layout={item.get('layout')}")
    if inspection.get("operators"):
        ops = inspection["operators"]
        print(f"Operator/layer types ({len(ops)}): " + ", ".join(f"{k}={v}" for k, v in list(ops.items())[:35]))
        if len(ops) > 35:
            print("  ... see JSON report for complete list")
    for warning in report.get("input_warnings", []):
        print("INPUT WARNING:", warning)
    for note in inspection.get("notes", []) or []:
        print("NOTE:", note)
    if report.get("compiler"):
        print("Compiler found:", report["compiler"].get("found"), "|", report["compiler"].get("path"))
    compiler_run = report.get("compiler_run")
    if compiler_run:
        if compiler_run.get("error"):
            print("Compiler run ERROR:", compiler_run["error"])
        else:
            print("Compiler exit status:", compiler_run.get("return_code"))
            if compiler_run.get("first_unsupported_operator"):
                print("First unsupported operator:", compiler_run["first_unsupported_operator"])
            if compiler_run.get("created_nnpa_files"):
                print("Generated NNPA files:", ", ".join(compiler_run["created_nnpa_files"]))
            if compiler_run.get("log_file"):
                print("Compiler log:", compiler_run["log_file"])
    print("FINAL: Compatibility is NOT CONFIRMED until the model is accepted by this NNPA compiler and tested on hardware.")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default=str(DEFAULT_CONFIG), help="JSON configuration file.")
    parser.add_argument("--model", help="Model path override; no source-code edits required.")
    parser.add_argument("--search-root", action="append", default=[], help="Add a model search directory (repeatable).")
    parser.add_argument("--list-candidates", action="store_true", help="List discovered YOLO files and exit.")
    parser.add_argument("--check-compiler", action="store_true", help="Check compiler executable path; do not compile.")
    parser.add_argument("--run-compiler", action="store_true", help="Run only the explicit command in config.compiler.command.")
    parser.add_argument("--json-report", help="Write a JSON report to this path.")
    args = parser.parse_args(argv)
    try:
        config_path = expand_path(args.config)
        config = load_config(config_path)
    except ValueError as exc:
        print(f"CONFIG ERROR: {exc}", file=sys.stderr)
        return 2

    candidates = discover_candidates(config, args.search_root)
    if args.list_candidates:
        if candidates:
            print("YOLO candidates:")
            for candidate in candidates:
                print(f"{candidate.stat().st_size:>12} bytes  {candidate}")
        else:
            print("No YOLO-named model candidates found in configured/search roots.")
        return 0

    chosen_raw = args.model or os.environ.get("COLIBRY_YOLO_MODEL") or config.get("model_path", "")
    if chosen_raw:
        model_path = expand_path(chosen_raw)
    elif candidates:
        model_path = candidates[0]
        if len(candidates) > 1:
            print(f"Found {len(candidates)} candidates; checking first alphabetically: {model_path}")
            print("Pass --model PATH to select another candidate explicitly.")
    else:
        print("No YOLO model selected or discovered.", file=sys.stderr)
        print("Pass --model PATH, set COLIBRY_YOLO_MODEL, or add a search root.", file=sys.stderr)
        return 2
    if not model_path.is_file():
        print(f"MODEL ERROR: file not found: {model_path}", file=sys.stderr)
        return 2
    if model_path.suffix.lower() not in SUPPORTED_SUFFIXES:
        print(f"MODEL ERROR: unsupported file extension: {model_path.suffix}", file=sys.stderr)
        return 2

    target = dict(config["target"])
    inspection = inspect_model(model_path)
    report: dict[str, Any] = {
        "model": str(model_path),
        "file_size_bytes": model_path.stat().st_size,
        "inspection": inspection,
        "target": target,
        "input_warnings": assess_inputs(inspection, target) if inspection.get("inspected") else [],
        "compiler": check_compiler(config) if args.check_compiler or args.run_compiler else None,
        "compiler_run": None,
    }
    if args.run_compiler:
        try:
            report["compiler_run"] = run_compiler(config, model_path)
        except (ValueError, OSError, subprocess.TimeoutExpired) as exc:
            report["compiler_run"] = {"error": f"{type(exc).__name__}: {exc}"}
    print_report(report)
    if args.json_report:
        report_path = expand_path(args.json_report)
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(report, indent=2, default=str) + "\n", encoding="utf-8")
        print("JSON report saved:", report_path)
    compiler_run = report.get("compiler_run")
    if args.run_compiler and (
        not compiler_run or compiler_run.get("error") or compiler_run.get("return_code", 1) != 0
    ):
        return 1
    if inspection.get("inspected") or (compiler_run and compiler_run.get("return_code") == 0):
        return 0
    return 3


if __name__ == "__main__":
    raise SystemExit(main())
