# Colibry Smart Video Sensor dashboard scaffold

This is a first host-side dashboard/API scaffold for the future live people-count feature.

**Important:** it is not connected to the experimental detector or physical EvalboardTiny yet. The current detector's validation metrics are not deployment-ready. The UI intentionally reports **device not connected** until it receives fresh data.

## Features

- Web dashboard with current-frame person count and connection status.
- API status goes offline after a configurable period without updates.
- Endpoint for a validated inference process to submit person detections.
- Lightweight recent count history API.
- Future integration points for camera preview and stable tracking IDs.

The current person_count means people visible in a single frame. For counting distinct people crossing an area over time, integrate a tracker and count unique track IDs or line-crossing events separately.

## Run locally in Ubuntu / UTM

From the repository root:

```bash
"$HOME/venvs/colibry-compiler/bin/python" -m pip install fastapi uvicorn
"$HOME/venvs/colibry-compiler/bin/python" dashboard/app.py
```

Open on the same Ubuntu desktop:

```bash
xdg-open http://127.0.0.1:8000
```

Default host is 127.0.0.1 intentionally. Do not expose this unauthenticated prototype to the public internet. For another local port, set COLIBRY_DASHBOARD_PORT. The stale-device timeout defaults to five seconds and can be set with COLIBRY_DEVICE_STALE_SECONDS.

## API

- GET /api/status — connection status, current count, boxes and update time.
- GET /api/history — recent count samples held in memory.
- POST /api/detections — submit the latest inference result.

Example local test update (not real inference):

```bash
curl -X POST http://127.0.0.1:8000/api/detections \
  -H 'Content-Type: application/json' \
  -d '{"person_count":1,"source":"manual-smoke-test","detections":[{"x1":0.2,"y1":0.1,"x2":0.6,"y2":0.9,"confidence":0.91,"track_id":1}]}'
```

Refresh the dashboard. It should show one person until updates become stale. To reset to offline, stop the server and restart it.

## Planned integration

1. Validate and export a detector that passes host-side tests.
2. Add a host/board inference adapter that converts normalized person boxes into the POST /api/detections schema.
3. Add stable person tracking to prevent the same person being counted repeatedly across frames.
4. Add a real camera preview only after the capture pipeline is integrated.
5. Add authentication before exposing the service beyond localhost.

No firmware, board flash, or model deployment is performed by this dashboard scaffold.
