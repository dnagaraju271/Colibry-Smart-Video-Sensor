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
            / "models/colibry_yolo/compression_trial/nnpa_output_featuremap_raw.bin"
        ),
        help="Raw 20,480-byte output dumped from the NNPA.",
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
    if len(raw_bytes) != EXPECTED_OUTPUT_BYTES:
        print(
            f"ERROR: NNPA output should be {EXPECTED_OUTPUT_BYTES} bytes, got {len(raw_bytes)}.",
            file=sys.stderr,
        )
        return 2

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

    # Model input is float [1,3,128,160] with values normalized to [0,1].
    input_u8 = np.frombuffer(input_bytes, dtype=np.uint8).reshape(1, 3, 128, 160)
    model_input = input_u8.astype(np.float32) / 255.0
    raw_output_u8 = np.frombuffer(raw_bytes, dtype=np.uint8)
    hardware_float = (raw_output_u8.astype(np.float32) - zero_point) * scale

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

    # The ONNX graph is NCHW [1,64,16,20]. NNPA size registers describe
    # width=20, height=16, depth=64, so test HWC linear order first, and also
    # report NCHW linear order to help diagnose a memory-layout mismatch.
    if reference.ndim == 4 and reference.shape == (1, 64, 16, 20):
        reference_hwc = reference.transpose(0, 2, 3, 1).reshape(-1)
        reference_nchw = reference.reshape(-1)
    elif reference.ndim == 4 and reference.shape == (1, 16, 20, 64):
        reference_hwc = reference.reshape(-1)
        reference_nchw = reference.transpose(0, 3, 1, 2).reshape(-1)
    else:
        reference_hwc = reference.reshape(-1)
        reference_nchw = reference.reshape(-1)

    hwc_metrics = summarize(hardware_float, reference_hwc)
    nchw_metrics = summarize(hardware_float, reference_nchw)

    print("========== NNPA vs ONNX REFERENCE ==========")
    print(f"ONNX model:                     {model_path}")
    print(f"ONNX Runtime:                  {ort.__version__}")
    print(f"Input tensor shape:             {tuple(model_input.shape)}")
    print(f"Reference output shape:         {tuple(reference.shape)}")
    print(f"Raw NNPA output bytes:           {len(raw_bytes)}")
    print(f"Output quantization source:      {q_source}")
    print(f"Output dequantization formula:   (raw - {zero_point}) * {scale:.10g}")
    print(f"NNPA dequantized min/max:        {hardware_float.min():.8g} / {hardware_float.max():.8g}")
    print(f"ONNX reference min/max:          {reference.min():.8g} / {reference.max():.8g}")
    print(f"ONNX reference mean/std:         {reference.mean():.8g} / {reference.std():.8g}")
    print("--- Candidate output layouts ---")
    for name, metrics in (("HWC (register width-height-depth)", hwc_metrics),
                          ("NCHW (ONNX native flatten)", nchw_metrics)):
        print(
            f"{name}: MAE={metrics['mae']:.8g}, RMSE={metrics['rmse']:.8g}, "
            f"max_abs={metrics['max_abs']:.8g}, corr={metrics['correlation']:.6f}, "
            f"cosine={metrics['cosine']:.6f}"
        )
    print("Quantization parameters came from the ONNX final DequantizeLinear node.")
    print("No detection accuracy is inferred from this comparison alone.")
    print("=============================================")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
