#!/usr/bin/env python3
"""Apply a guarded SPIS DMA finalization patch to the local ASYGN SDK source.

This edits source files only. It creates timestamped backups and never builds,
loads firmware, writes external flash, or touches the connected board. Run with
--apply only after reviewing the project patch notes.
"""
from __future__ import annotations

import argparse
from datetime import datetime
from pathlib import Path
import sys


def replace_exact(text: str, old: str, new: str, label: str, expected: int = 1) -> str:
    count = text.count(old)
    if count != expected:
        raise RuntimeError(
            f"{label}: expected exactly {expected} matching anchor(s), found {count}; "
            "no source files have been changed."
        )
    return text.replace(old, new)


def transform(sdk_root: Path) -> dict[Path, str]:
    spis_c = sdk_root / "components_drivers/SPIS_PROTOCOL/spis_protocol.c"
    spis_h = sdk_root / "components_drivers/SPIS_PROTOCOL/spis_protocol.h"
    main_c = sdk_root / "examples/20_hello_world/main.c"

    for path in (spis_c, spis_h, main_c):
        if not path.is_file():
            raise FileNotFoundError(f"Required SDK source file not found: {path}")

    c = spis_c.read_text()
    h = spis_h.read_text()
    m = main_c.read_text()

    c = replace_exact(
        c,
        "uint32_t curr_opcode = 0;\n"
        "uint8_t  argument_cnt = 0;\n"
        "uint8_t  alloc_dma_channel = 0;\n"
        "volatile uint32_t spis_dma_rx_drain_count = 0;",
        "#define SPIS_BURST_FINALIZE_TIMEOUT_US 100000u\n"
        "uint32_t curr_opcode = 0;\n"
        "uint8_t  argument_cnt = 0;\n"
        "uint8_t  alloc_dma_channel = 0;\n"
        "volatile uint32_t spis_dma_rx_drain_count = 0;\n"
        "volatile uint8_t spis_burst_finalize_timed_out = 0;",
        "SPIS globals/timeout definition",
    )

    dma_enable_old = "DMA_reg->dma_int_en.word = (1 << alloc_dma_channel);"
    dma_enable_new = (
        "DMA_reg->dma_int_en.word =\n"
        "            (1u << alloc_dma_channel) |\n"
        "            (1u << (DMA_EVT_READ_ERROR_OFFSET + alloc_dma_channel)) |\n"
        "            (1u << (DMA_EVT_WRITE_ERROR_OFFSET + alloc_dma_channel));"
    )
    c = replace_exact(c, dma_enable_old, dma_enable_new, "DMA done/read-error/write-error enables", expected=2)

    old_finalize = (
        "void spis_burst_finalize()\n"
        "{\n"
        "  while (SPIS_reg->spis_status.fields.spis_tx_fifo_empty == 0 ||\n"
        "         SPIS_reg->spis_status.fields.spis_busy != 0) {\n"
        "    colibrySCU->delay_us(1);\n"
        "  }\n"
        "  SPIS_reg->spis_ctrl.fields.spis_dma_mode = 0;\n"
        "  // Reactivate SPIS interrupts\n"
        "  colibrySPIS->spis_rx_irq_en_set(1);\n"
        "}"
    )
    new_finalize = (
        "void spis_burst_finalize()\n"
        "{\n"
        "  uint32_t waited_us = 0;\n"
        "  spis_burst_finalize_timed_out = 0;\n"
        "  while (SPIS_reg->spis_status.fields.spis_tx_fifo_empty == 0 ||\n"
        "         SPIS_reg->spis_status.fields.spis_busy != 0) {\n"
        "    if (waited_us >= SPIS_BURST_FINALIZE_TIMEOUT_US) {\n"
        "      spis_burst_finalize_timed_out = 1;\n"
        "      break;\n"
        "    }\n"
        "    colibrySCU->delay_us(1);\n"
        "    ++waited_us;\n"
        "  }\n"
        "  // Always release DMA-drain mode so a stuck peripheral cannot trap the CPU\n"
        "  // forever in finalization. The timeout flag is reported by main context.\n"
        "  SPIS_reg->spis_ctrl.fields.spis_dma_mode = 0;\n"
        "  // Reactivate SPIS interrupts\n"
        "  colibrySPIS->spis_rx_irq_en_set(1);\n"
        "}"
    )
    c = replace_exact(c, old_finalize, new_finalize, "bounded SPIS finalizer")

    h = replace_exact(
        h,
        "void spis_burst_finalize();",
        "void spis_burst_finalize();\n"
        "extern volatile uint8_t spis_burst_finalize_timed_out;",
        "SPIS finalizer timeout declaration",
    )

    m = replace_exact(
        m,
        "volatile uint8_t nnpa_inference_done = 0;\n\nColibryBoard colibryBoard;",
        "volatile uint8_t nnpa_inference_done = 0;\n"
        "volatile uint8_t spis_dma_finalize_pending = 0;\n"
        "volatile uint32_t spis_dma_error_status = 0;\n\n"
        "ColibryBoard colibryBoard;",
        "main-context finalizer flags",
    )

    old_irq = (
        "irq_handler_t dma_irq_handler(void) {\n"
        "  if (DMA_reg->dma_int_status.word & (1 << SPIS_DMA_CHANNEL)) {\n"
        "    DMA_reg->dma_int_clear.word = (1 << SPIS_DMA_CHANNEL);\n"
        "    spis_burst_finalize();\n"
        "  }\n"
        "  if (DMA_reg->dma_int_status.word & (1 << IMGRX_DMA_CHANNEL)) {\n"
        "    DMA_reg->dma_int_clear.word = (1 << IMGRX_DMA_CHANNEL);\n"
        "    dma_irq_reached = 1;\n"
        "  }\n"
        "}"
    )
    new_irq = (
        "irq_handler_t dma_irq_handler(void) {\n"
        "  uint32_t dma_status = DMA_reg->dma_int_status.word;\n"
        "  uint32_t spis_done_mask = (1u << SPIS_DMA_CHANNEL);\n"
        "  uint32_t spis_read_error_mask = (1u << (DMA_EVT_READ_ERROR_OFFSET + SPIS_DMA_CHANNEL));\n"
        "  uint32_t spis_write_error_mask = (1u << (DMA_EVT_WRITE_ERROR_OFFSET + SPIS_DMA_CHANNEL));\n"
        "  uint32_t spis_error_mask = spis_read_error_mask | spis_write_error_mask;\n"
        "  uint32_t spis_events = dma_status & (spis_done_mask | spis_error_mask);\n"
        "\n"
        "  if (spis_events != 0u) {\n"
        "    DMA_reg->dma_int_clear.word = spis_events;\n"
        "    spis_dma_error_status |= spis_events & spis_error_mask;\n"
        "    spis_dma_finalize_pending = 1;\n"
        "  }\n"
        "\n"
        "  if (dma_status & (1u << IMGRX_DMA_CHANNEL)) {\n"
        "    DMA_reg->dma_int_clear.word = (1u << IMGRX_DMA_CHANNEL);\n"
        "    dma_irq_reached = 1;\n"
        "  }\n"
        "}"
    )
    m = replace_exact(m, old_irq, new_irq, "DMA IRQ handler")

    old_loop = (
        "  while (1) {\n"
        "    if (colibrySCU->regmap->scu_core_msg_0.word == MSG_WAIT_INFERENCE) {"
    )
    new_loop = (
        "  while (1) {\n"
        "    // Finalize SPIS bursts in main context, not inside the DMA ISR. This lets\n"
        "    // the SPIS RX ISR continue draining dummy MOSI words during the transfer.\n"
        "    if (spis_dma_finalize_pending) {\n"
        "      uint32_t dma_errors = spis_dma_error_status;\n"
        "      spis_dma_finalize_pending = 0;\n"
        "      spis_dma_error_status = 0;\n"
        "      spis_burst_finalize();\n"
        "      if (dma_errors != 0u) {\n"
        "        printf(\"SPIS DMA read/write error event bits: 0x%08lx\\n\", (unsigned long)dma_errors);\n"
        "      }\n"
        "      if (spis_burst_finalize_timed_out) {\n"
        "        printf(\"ERROR: SPIS burst finalization timed out; protocol state may need SRAM firmware restart\\n\");\n"
        "      }\n"
        "    }\n"
        "\n"
        "    if (colibrySCU->regmap->scu_core_msg_0.word == MSG_WAIT_INFERENCE) {"
    )
    m = replace_exact(m, old_loop, new_loop, "main-loop deferred finalization")

    return {spis_c: c, spis_h: h, main_c: m}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--sdk-root",
        type=Path,
        default=Path.home() / "colibryNPU_SDK",
        help="Local vendor SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Create backups and apply the source changes. Without this flag, dry-run only.",
    )
    args = parser.parse_args()
    sdk_root = args.sdk_root.expanduser()

    try:
        changed = transform(sdk_root)
    except Exception as exc:
        print(f"ERROR: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 2

    print("SPIS DMA recovery patch preflight passed.")
    for path, content in changed.items():
        print(f"  {path}")
        print(f"    original bytes={path.stat().st_size}, patched bytes={len(content.encode())}")
    if not args.apply:
        print("DRY RUN ONLY: no files changed. Re-run with --apply to back up and patch.")
        return 0

    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    backups: list[tuple[Path, Path]] = []
    try:
        for path, content in changed.items():
            backup = path.with_name(f"{path.name}.pre_spis_recovery_{stamp}")
            backup.write_bytes(path.read_bytes())
            backups.append((path, backup))
        for path, content in changed.items():
            path.write_text(content)
    except Exception as exc:
        print(f"ERROR applying patch: {type(exc).__name__}: {exc}", file=sys.stderr)
        print("Backups already created:")
        for original, backup in backups:
            print(f"  {original} <- {backup}")
        return 3

    print("PATCH APPLIED. Timestamped backups:")
    for original, backup in backups:
        print(f"  {original} <- {backup}")
    print("No build, firmware load, or flash operation was performed.")
    print("Next: review the diff and confirm the example build instructions before compiling.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
