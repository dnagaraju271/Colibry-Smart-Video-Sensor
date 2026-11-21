# INT8 and NNPA model export

The previous own-count CNN was converted to full-integer uint8 TFLite and compiled using Colibry NPU compiler 0.2.1.

Current locally verified artifacts (2026-11-16):

| Artifact | Size | SHA-256 |
|---|---:|---|
| PTQ TFLite `own_count_cnn_uint8.tflite` | 17,312 bytes | `df8d84655a0c1142aa556120920587ce256f26bd14a0c0c5a8c598bfbfe9a45a` |
| NNPA `own_count_cnn_uint8.tflite.nnpa` | 19,392 bytes | `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e` |

Input metadata: uint8 shape `[1,30,40,1]`, scale approximately 1/255, zero point 0.
Output metadata: uint8 shape `[1,3]`, scale `0.024000564590096474`, zero point 241.

The board demo generator checks the NNPA size and SHA-256 before embedding it in the SRAM firmware.

## Four-class retrain status

The artifact above remains the previous three-class model with output shape
`[1,3]`. It is **not compatible** with four-class labels and must not be used
to claim 4-person prediction.

The updated host training model and legacy exporter now target output shape
`[1,4]`. After training the four-class model, regenerate the uint8 TFLite
artifact, compile a new NNPA bitstream, record its new size and SHA-256, and
only then update the firmware's embedded-header checks and four-class output
parser. The old artifact and its hashes remain documented for traceability.

These artifacts are local under `~/models/edgeguard_own_count/ptq/`; model binaries and weights are not committed to GitHub. Re-check local SHA-256 values before use. The compile artifact is for inference only.
