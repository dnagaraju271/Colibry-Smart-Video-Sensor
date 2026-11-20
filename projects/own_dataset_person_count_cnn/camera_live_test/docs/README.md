# Camera and ISP diagnostic references

This directory is the maintained engineering notebook for the PAG7920LT → CAMIF → Colibry ISP scaler → NNPA live-camera test.

## Documents

- [Continuous PAG7920LT FPS tests — 2026-11-20](continuous-camera-fps-tests-2026-11-20.md) — measured CAMIF frame counts, register snapshots, and unresolved image-transfer limitations.

- [ISP register and signal reference](isp-register-reference.md) — register fields, masks, HAL comments, interpretation, and original SDK source paths.
- [Current capture investigation](capture-investigation-2026-11-18.md) — measured UART evidence, what it proves, what remains unknown, and the next safe checks.

## Rules for future updates

1. Record measured values exactly as printed; distinguish observations from hypotheses.
2. Include test date, firmware commit/hash when known, board/sensor, input scene, and exact commands.
3. Do not mark a test successful solely because a frame counter or NNPA byte count advanced. Validate image content and all relevant error flags.
4. Preserve prior test records. Add a new dated investigation file or append a dated section rather than silently rewriting history.
5. Do not copy vendor SDK source files or proprietary manuals into this repository. Cite their SDK paths and record only the small definitions/comments needed to explain our measurements.
6. Keep generated UART logs and images as test artifacts when practical, but do not commit large build products or binaries. A compact report and reproducible decoder command should remain in Git.

## Scope

These notes support the existing small CNN and camera bring-up work. They do not authorize adding unrelated features or changing the professor's project scope.
