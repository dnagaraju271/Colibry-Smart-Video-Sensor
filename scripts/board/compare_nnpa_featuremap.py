#!/usr/bin/env python3
"""Compare a saved Colibry NNPA feature map against ONNX Runtime output.

The script does not trigger inference or write to the board. By default it reads
the output quantization registers from the live board over SPIS, then runs the
same input through the supplied reference ONNX model and compares both output
orders. The NNPA raw file must have been captured from this same input/model.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import numpy as np

NNPA_REG_BASE = 0x30000000
REG_OUTPUT_SCALING = 0x4C
REG_OUTPUT_OFFSET = 0x50
EXPECTED_INPUT_BYTES = 3 * 128 * 160
EXPECTED_OUTPUT_BYTES = 64 * 16 * 20


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare NNPA raw feature-map bytes with an ONNX Runtime reference."
    )
    parser.add_argument(
        "--sdk-root",
        type=Path,
        default=Path.home() / "colibryNPU_SDK",
        help="Local ColibryNPU SDK root (default: ~/colibryNPU_SDK).",
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
    parser.add_argument(
        "--port",
        default=None,
        help="Optional FTDI URI for SPIS, e.g. ftdi://ftdi:4232hp:3:4/2.",
    )
    parser.add_argument(
        "--output-scale",
        type=float,
        default=None,
        help="Optional scale override; if set with zero-point, avoids board register reads.",
    )
    parser.add_argument(
        "--output-zero-point",
        type=int,
        default=None,
        help="Optional zero-point override; must be paired with --output-scale.",
    )
    return parser.parse_args()


def load_output_quantization(args: argparse.Namespace) -> tuple[float, int, str]:
    if (args.output_scale is None) != (args.output_zero_point is None):
        raise ValueError("--output-scale and --output-zero-point must be provided together.")

    if args.output_scale is not None:
        scale = float(args.output_scale)
        zero_point = int(args.output_zero_point)
        source = "command-line override"
    else:
        sdk_root = args.sdk_root.expanduser()
        candidates = (
            sdk_root / "riscv_runtime" / "tools",
            sdk_root / "tools",
        )
        for candidate in candidates:
            if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
                sys.path.insert(0, str(candidate))
                break
        else:
            raise FileNotFoundError(
                "Could not locate colibry_interfaces/colibry_spis_api.py under "
                f"{sdk_root}"
            )

        from colibry_interfaces.colibry_spis_api import Colibry_spis_api

        # Read-only register access; no inference trigger, reset, or flash write.
        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)
        scale_reg = ll.mmap_read(NNPA_REG_BASE + REG_OUTPUT_SCALING)
        offset_reg = ll.mmap_read(NNPA_REG_BASE + REG_OUTPUT_OFFSET)
        scale = scale_reg / 65536.0
        # Match the SDK reference code: int16_t(output_offset_register >> 8).
        zero_point = (offset_reg >> 8) & 0xFFFF
        if zero_point >= 0x8000:
            zero_point -= 0x10000
        source = (
            f"live board registers (scaling=0x{scale_reg:08X}, "
            f"offset=0x{offset_reg:08X})"
        )

    if not np.isfinite(scale) or scale <= 0:
        raise ValueError(f"Invalid output quantization scale: {scale}")
    if zero_point < -32768 or zero_point > 32767:
        raise ValueError(f"Invalid signed 16-bit output zero point: {zero_point}")
    return scale, zero_point, source


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
        scale, zero_point, q_source = load_output_quantization(args)
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
    hardware_float = (raw_output_u8.astype(np.float32) + zero_point) * scale

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
    print(f"Output dequantization formula:   (raw + {zero_point}) * {scale:.10g}")
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
    print("No detection accuracy is inferred from this comparison alone.")
    print("=============================================")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
