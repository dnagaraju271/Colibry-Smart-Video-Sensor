#!/usr/bin/env python3
"""Compare a saved Colibry NNPA feature map against ONNX Runtime output.

Output quantization is read from the model's final DequantizeLinear node, not
from live board registers. This avoids interpreting an invalid SPIS read as a
scale. This script does not access or modify the board.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import numpy as np

EXPECTED_INPUT_BYTES = 3 * 128 * 160
EXPECTED_OUTPUT_BYTES = 64 * 16 * 20


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare NNPA raw feature-map bytes with an ONNX Runtime reference."
    )
    parser.add_argument(
        "--input",
        type=Path,
        default=(
            Path.home()
            / "models/colibry_yolo/compression_trial/test_person_input_nchw_u8.bin"
        ),
        help="Original uint8 NCHW input tensor (61,440 bytes).",
    )
    parser.add_argument(
        "--raw-output",
        type=Path,
        default=(
            Path.home()
            / "models/colibry_yolo/compression_trial/nnpa_output_memory_window4.bin"
        ),
        help="20,480-byte legacy output or 81,920-byte four-lane diagnostic window.",
    )
    parser.add_argument(
        "--model",
        type=Path,
        default=(
            Path.home()
            / "models/colibry_yolo/compression_trial/"
            / "yolov8n_layer4_qdq_conv_add_int8_terminal.onnx"
        ),
        help="Float-input ONNX reference model.",
    )
    return parser.parse_args()


def load_output_quantization(model_path: Path) -> tuple[float, int, str]:
    """Read per-tensor quantization parameters for the graph's final DQ output."""
    try:
        import onnx
        from onnx import numpy_helper
    except ImportError as exc:
        raise RuntimeError(
            "The 'onnx' package is required to read the model's Q/DQ parameters."
        ) from exc

    model = onnx.load(str(model_path))
    if not model.graph.output:
        raise ValueError("ONNX graph has no declared outputs.")

    output_name = model.graph.output[0].name
    dq = next(
        (
            node for node in model.graph.node
            if node.op_type == "DequantizeLinear" and output_name in node.output
        ),
        None,
    )
    if dq is None:
        raise ValueError(
            f"Graph output {output_name!r} is not produced directly by "
            "DequantizeLinear; cannot safely infer output quantization."
        )
    if len(dq.input) < 2:
        raise ValueError("Final DequantizeLinear node has no scale input.")

    initializers = {
        tensor.name: numpy_helper.to_array(tensor)
        for tensor in model.graph.initializer
    }
    constant_nodes = {
        output: node
        for node in model.graph.node if node.op_type == "Constant"
        for output in node.output
    }

    def constant_value(name: str):
        if name in initializers:
            return initializers[name]
        node = constant_nodes.get(name)
        if node is not None:
            for attr in node.attribute:
                if attr.name == "value" and attr.HasField("t"):
                    return numpy_helper.to_array(attr.t)
        return None

    scale_array = constant_value(dq.input[1])
    if scale_array is None:
        raise ValueError(
            f"Cannot find constant scale tensor {dq.input[1]!r} for final DQ."
        )
    scale_array = np.asarray(scale_array)
    if scale_array.size != 1:
        raise ValueError(
            f"Final DQ scale has {scale_array.size} values; per-channel output "
            "quantization is not handled by this diagnostic."
        )
    scale = float(scale_array.reshape(-1)[0])

    if len(dq.input) >= 3 and dq.input[2]:
        zero_array = constant_value(dq.input[2])
        if zero_array is None:
            raise ValueError(
                f"Cannot find constant zero-point tensor {dq.input[2]!r} for final DQ."
            )
        zero_array = np.asarray(zero_array)
        if zero_array.size != 1:
            raise ValueError(
                f"Final DQ zero-point has {zero_array.size} values; expected one."
            )
        zero_point = int(zero_array.reshape(-1)[0])
    else:
        zero_point = 0

    if not np.isfinite(scale) or scale <= 0:
        raise ValueError(f"Invalid output quantization scale: {scale}")
    return scale, zero_point, (
        f"ONNX final DequantizeLinear node {dq.name or '(unnamed)'} "
        f"(scale tensor={dq.input[1]!r}, zero-point tensor="
        f"{dq.input[2] if len(dq.input) >= 3 else '(default 0)'!r})"
    )

def summarize(candidate: np.ndarray, reference: np.ndarray) -> dict[str, float]:
    delta = candidate.astype(np.float64) - reference.astype(np.float64)
    candidate64 = candidate.astype(np.float64)
    reference64 = reference.astype(np.float64)
    candidate_centered = candidate64 - candidate64.mean()
    reference_centered = reference64 - reference64.mean()
    denom = np.linalg.norm(candidate_centered) * np.linalg.norm(reference_centered)
    corr = (
        float(np.dot(candidate_centered, reference_centered) / denom)
        if denom > 0
        else float("nan")
    )
    ref_norm = np.linalg.norm(reference64)
    cosine = (
        float(np.dot(candidate64, reference64) / (np.linalg.norm(candidate64) * ref_norm))
        if np.linalg.norm(candidate64) > 0 and ref_norm > 0
        else float("nan")
    )
    return {
        "mae": float(np.mean(np.abs(delta))),
        "rmse": float(np.sqrt(np.mean(delta * delta))),
        "max_abs": float(np.max(np.abs(delta))),
        "correlation": corr,
        "cosine": cosine,
    }


def main() -> int:
    args = parse_args()
    input_path = args.input.expanduser()
    raw_output_path = args.raw_output.expanduser()
    model_path = args.model.expanduser()

    for path in (input_path, raw_output_path, model_path):
        if not path.is_file():
            print(f"ERROR: required file does not exist: {path}", file=sys.stderr)
            return 2

    input_bytes = input_path.read_bytes()
    raw_bytes = raw_output_path.read_bytes()
    if len(input_bytes) != EXPECTED_INPUT_BYTES:
        print(
            f"ERROR: input should be {EXPECTED_INPUT_BYTES} bytes, got {len(input_bytes)}.",
            file=sys.stderr,
        )
        return 2
    allowed_raw_lengths = (EXPECTED_OUTPUT_BYTES, EXPECTED_OUTPUT_BYTES * 4)
    if len(raw_bytes) not in allowed_raw_lengths:
        print(
            "ERROR: output file should contain either "
            f"{EXPECTED_OUTPUT_BYTES} bytes (legacy tensor dump) or "
            f"{EXPECTED_OUTPUT_BYTES * 4} bytes (four-lane diagnostic window); "
            f"got {len(raw_bytes)}.",
            file=sys.stderr,
        )
        return 2
    diagnostic_window = len(raw_bytes) == EXPECTED_OUTPUT_BYTES * 4

    try:
        import onnxruntime as ort
    except ImportError:
        print(
            "ERROR: onnxruntime is not installed in this Python environment. "
            "Install it in the same venv, or paste this error before proceeding.",
            file=sys.stderr,
        )
        return 3

    try:
        scale, zero_point, q_source = load_output_quantization(model_path)
    except Exception as exc:
        print(
            f"ERROR: unable to determine output quantization: "
            f"{type(exc).__name__}: {exc}",
            file=sys.stderr,
        )
        return 4

    input_u8 = np.frombuffer(input_bytes, dtype=np.uint8).reshape(1, 3, 128, 160)
    model_input = input_u8.astype(np.float32) / 255.0

    try:
        session = ort.InferenceSession(str(model_path), providers=["CPUExecutionProvider"])
        inputs = session.get_inputs()
        if len(inputs) != 1:
            raise ValueError(f"Expected one ONNX input, found {len(inputs)}.")
        input_name = inputs[0].name
        output_values = session.run(None, {input_name: model_input})
        if not output_values:
            raise ValueError("ONNX Runtime returned no outputs.")
        reference = np.asarray(output_values[0], dtype=np.float32)
    except Exception as exc:
        print(
            f"ERROR: ONNX Runtime reference failed: {type(exc).__name__}: {exc}",
            file=sys.stderr,
        )
        return 5

    if reference.size != EXPECTED_OUTPUT_BYTES:
        print(
            f"ERROR: ONNX output has shape {reference.shape} and {reference.size} "
            f"elements, expected {EXPECTED_OUTPUT_BYTES}.",
            file=sys.stderr,
        )
        return 5

    if reference.ndim == 4 and reference.shape == (1, 64, 16, 20):
        reference_hwc = reference.transpose(0, 2, 3, 1).reshape(-1)
        reference_nchw = reference.reshape(-1)
    elif reference.ndim == 4 and reference.shape == (1, 16, 20, 64):
        reference_hwc = reference.reshape(-1)
        reference_nchw = reference.transpose(0, 3, 1, 2).reshape(-1)
    else:
        reference_hwc = reference.reshape(-1)
        reference_nchw = reference.reshape(-1)

    expected_codes_hwc = np.clip(
        np.rint(reference_hwc / scale + zero_point), 0, 255
    ).astype(np.uint8)
    expected_codes_nchw = np.clip(
        np.rint(reference_nchw / scale + zero_point), 0, 255
    ).astype(np.uint8)

    print("========== NNPA vs ONNX REFERENCE ==========")
    print(f"ONNX model:                     {model_path}")
    print(f"ONNX Runtime:                  {ort.__version__}")
    print(f"Input tensor shape:             {tuple(model_input.shape)}")
    print(f"Reference output shape:         {tuple(reference.shape)}")
    print(f"Saved output bytes:             {len(raw_bytes)}")
    print(f"Raw memory interpretation:      {'four lanes / 4-byte element hypothesis' if diagnostic_window else 'legacy one-byte-per-element capture'}")
    print(f"Output quantization source:      {q_source}")
    print(f"Output quantization:             scale={scale:.10g}, zero_point={zero_point}")
    print(f"ONNX reference min/max:          {reference.min():.8g} / {reference.max():.8g}")
    print(f"ONNX reference mean/std:         {reference.mean():.8g} / {reference.std():.8g}")
    print(
        f"Expected quantized code range:   {int(min(expected_codes_hwc.min(), expected_codes_nchw.min()))} "
        f".. {int(max(expected_codes_hwc.max(), expected_codes_nchw.max()))}"
    )

    if diagnostic_window:
        lanes = np.frombuffer(raw_bytes, dtype=np.uint8).reshape(EXPECTED_OUTPUT_BYTES, 4)
        lane_results = []
        print("--- Four-byte lane diagnostic (no layout assumed) ---")
        for lane in range(4):
            codes = lanes[:, lane]
            dequant = (codes.astype(np.float32) - zero_point) * scale
            hist = np.bincount(codes, minlength=256)
            common = sorted(
                ((int(count), int(value)) for value, count in enumerate(hist)),
                reverse=True,
            )[:5]
            stats = {
                "lane": lane,
                "min": int(codes.min()),
                "max": int(codes.max()),
                "mean": float(codes.mean()),
                "std": float(codes.std()),
                "below_zp": float(np.mean(codes < zero_point)),
                "hwc": summarize(dequant, reference_hwc),
                "nchw": summarize(dequant, reference_nchw),
                "codes_hwc": summarize(codes, expected_codes_hwc),
                "codes_nchw": summarize(codes, expected_codes_nchw),
                "common": common,
            }
            lane_results.append(stats)
            print(
                f"Lane {lane} stats min/max/mean/std: "
                f"{stats['min']}/{stats['max']}/{stats['mean']:.5g}/{stats['std']:.5g}; "
                f"below zero-point={stats['below_zp']:.3%}"
            )
            print(f"  Frequent codes: {common}")
            print(
                f"  vs HWC ref: MAE={stats['hwc']['mae']:.6g}, "
                f"RMSE={stats['hwc']['rmse']:.6g}, "
                f"corr={stats['hwc']['correlation']:.6f}; "
                f"quant-code MAE={stats['codes_hwc']['mae']:.5g}"
            )
            print(
                f"  vs NCHW ref: MAE={stats['nchw']['mae']:.6g}, "
                f"RMSE={stats['nchw']['rmse']:.6g}, "
                f"corr={stats['nchw']['correlation']:.6f}; "
                f"quant-code MAE={stats['codes_nchw']['mae']:.5g}"
            )
        best = min(
            lane_results,
            key=lambda x: min(x["hwc"]["rmse"], x["nchw"]["rmse"])
        )
        print(
            f"Best single-lane candidate by RMSE: lane {best['lane']} "
            f"(HWC RMSE={best['hwc']['rmse']:.6g}, "
            f"NCHW RMSE={best['nchw']['rmse']:.6g})."
        )
        print(
            "Lane selection is diagnostic only; it does not establish the NNPA's "
            "documented output format."
        )
    else:
        raw_codes = np.frombuffer(raw_bytes, dtype=np.uint8)
        hardware_float = (raw_codes.astype(np.float32) - zero_point) * scale
        hwc_metrics = summarize(hardware_float, reference_hwc)
        nchw_metrics = summarize(hardware_float, reference_nchw)
        print("--- Legacy flat-byte comparison ---")
        for name, metrics in (("HWC", hwc_metrics), ("NCHW", nchw_metrics)):
            print(
                f"{name}: MAE={metrics['mae']:.8g}, RMSE={metrics['rmse']:.8g}, "
                f"max_abs={metrics['max_abs']:.8g}, corr={metrics['correlation']:.6f}, "
                f"cosine={metrics['cosine']:.6f}"
            )

    print("No detection accuracy is inferred from this comparison alone.")
    print("=============================================")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
