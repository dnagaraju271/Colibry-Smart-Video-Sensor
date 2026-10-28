"""Edge dashboard API and web UI scaffold for Colibry Smart Video Sensor.

This service intentionally does not run the current experimental detector.
The detector has not passed validation yet. A trusted host/board inference
process may POST its latest person detections to /api/detections after it is
validated and integrated.
"""
from __future__ import annotations

import os
import time
from collections import deque
from threading import Lock
from typing import Any

from fastapi import FastAPI, HTTPException
from fastapi.responses import HTMLResponse
from pydantic import BaseModel, Field

DEVICE_STALE_SECONDS = float(os.getenv("COLIBRY_DEVICE_STALE_SECONDS", "5"))
HISTORY_LIMIT = 60
_lock = Lock()
_latest: dict[str, Any] = {
    "connected": False,
    "person_count": 0,
    "detections": [],
    "updated_at": None,
    "last_seen_monotonic": None,
    "source": "not connected",
}
_history: deque[dict[str, Any]] = deque(maxlen=HISTORY_LIMIT)

app = FastAPI(title="Colibry Smart Video Sensor Dashboard", version="0.1.0")


class Box(BaseModel):
    x1: float = Field(ge=0, le=1)
    y1: float = Field(ge=0, le=1)
    x2: float = Field(ge=0, le=1)
    y2: float = Field(ge=0, le=1)
    confidence: float = Field(ge=0, le=1)
    track_id: int | None = Field(default=None, ge=0)


class DetectionUpdate(BaseModel):
    # Count of people visible in this frame, after person filtering and NMS.
    # Once tracking is implemented, use distinct active person track IDs.
    person_count: int = Field(ge=0, le=1000)
    detections: list[Box] = Field(default_factory=list, max_length=1000)
    source: str = Field(default="host-inference", min_length=1, max_length=80)


def _snapshot() -> dict[str, Any]:
    with _lock:
        last_seen = _latest["last_seen_monotonic"]
        connected = last_seen is not None and time.monotonic() - last_seen <= DEVICE_STALE_SECONDS
        return {
            "connected": connected,
            "status": "connected" if connected else "device not connected",
            "person_count": _latest["person_count"] if connected else 0,
            "detections": _latest["detections"] if connected else [],
            "updated_at": _latest["updated_at"] if connected else None,
            "source": _latest["source"] if connected else "not connected",
            "stale_after_seconds": DEVICE_STALE_SECONDS,
        }


@app.get("/api/status")
def status() -> dict[str, Any]:
    return _snapshot()


@app.get("/api/history")
def history() -> dict[str, Any]:
    with _lock:
        return {"samples": list(_history), "limit": HISTORY_LIMIT}


@app.post("/api/detections")
def update_detections(update: DetectionUpdate) -> dict[str, Any]:
    # Reject invalid/inverted boxes instead of displaying malformed coordinates.
    for box in update.detections:
        if box.x2 <= box.x1 or box.y2 <= box.y1:
            raise HTTPException(status_code=422, detail="Each box must have x2>x1 and y2>y1")
    now = time.time()
    with _lock:
        _latest.update({
            "connected": True,
            "person_count": update.person_count,
            "detections": [box.model_dump() for box in update.detections],
            "updated_at": now,
            "last_seen_monotonic": time.monotonic(),
            "source": update.source,
        })
        _history.append({"timestamp": now, "person_count": update.person_count})
    return {"accepted": True, "person_count": update.person_count, "updated_at": now}


@app.get("/", response_class=HTMLResponse)
def dashboard() -> str:
    return HTML


HTML = r"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Colibry Smart Video Sensor</title>
<style>
:root{color-scheme:dark;--bg:#0b1020;--panel:#131b2e;--line:#273550;--text:#eaf0ff;--muted:#98a8c6;--accent:#59d3b4;--warn:#ffb86b}
*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at top,#172746 0,#0b1020 55%);font:15px/1.5 system-ui,-apple-system,Segoe UI,sans-serif;color:var(--text)}
main{max-width:1100px;margin:auto;padding:32px 20px}header{display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap;margin-bottom:24px}
h1{font-size:clamp(24px,4vw,34px);margin:0}p{color:var(--muted);margin:6px 0 0}.status{border:1px solid var(--line);border-radius:999px;padding:8px 13px;font-weight:650;display:flex;align-items:center;gap:9px}
.dot{width:9px;height:9px;border-radius:50%;background:var(--warn)}.connected .dot{background:var(--accent)}
.grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:16px}.panel{background:rgba(19,27,46,.94);border:1px solid var(--line);border-radius:18px;padding:22px;min-width:0}
.label{font-size:13px;color:var(--muted)}.number{font-size:clamp(38px,6vw,58px);font-weight:750;letter-spacing:-2px;margin:12px 0 0}.unit{color:var(--muted);font-size:13px}
.wide{grid-column:span 2}.section-title{font-size:17px;margin:0 0 14px}.placeholder{border:1px dashed #3b4c6c;border-radius:12px;min-height:210px;display:grid;place-items:center;text-align:center;padding:22px;color:var(--muted)}
.pill{display:inline-block;border:1px solid var(--line);border-radius:8px;padding:4px 8px;color:var(--muted);font-size:12px;margin-top:12px}
ul{margin:8px 0 0;padding-left:19px;color:var(--muted)}footer{margin-top:22px;color:var(--muted);font-size:12px}
@media(max-width:720px){.grid{grid-template-columns:1fr}.wide{grid-column:auto}}
</style>
</head>
<body><main>
<header><div><h1>Colibry Smart Video Sensor</h1><p>Embedded AI · Person detection · Live people count</p></div>
<div id="status" class="status"><span class="dot"></span><span id="statusText">Checking device…</span></div></header>
<div class="grid">
<section class="panel"><div class="label">PEOPLE IN CURRENT FRAME</div><div id="count" class="number">—</div><div class="unit">Visible person detections</div><span class="pill" id="updated">Waiting for device data</span></section>
<section class="panel"><div class="label">DEVICE CONNECTION</div><div id="connection" class="number" style="font-size:30px">Offline</div><p id="source">No inference updates received.</p></section>
<section class="panel"><div class="label">DETECTIONS</div><div id="detections" class="number">0</div><div class="unit">Boxes in latest frame</div></section>
<section class="panel wide"><h2 class="section-title">Camera / detection preview</h2><div class="placeholder"><div><strong>Camera stream not connected</strong><br><br>When inference and a camera stream are integrated, the preview and bounding boxes will appear here.<br><span class="pill">Preview integration pending</span></div></div></section>
<section class="panel"><h2 class="section-title">Next milestones</h2><ul><li>Validate person detector</li><li>Connect camera and inference output</li><li>Add stable tracking IDs and unique-person counting</li><li>Display historical counts and device health</li></ul></section>
</div>
<footer>Prototype dashboard · Count is zero when device updates become stale. The current experimental model is not automatically connected.</footer>
</main>
<script>
const $=id=>document.getElementById(id);
async function refresh(){
 try{
  const r=await fetch('/api/status',{cache:'no-store'});if(!r.ok)throw new Error('API unavailable');
  const d=await r.json();$('status').classList.toggle('connected',d.connected);
  $('statusText').textContent=d.status;$('connection').textContent=d.connected?'Online':'Offline';
  $('count').textContent=d.connected?d.person_count:0;
  $('detections').textContent=d.connected?d.detections.length:0;
  $('source').textContent=d.connected?'Source: '+d.source:'No recent inference updates received.';
  $('updated').textContent=d.updated_at?'Last update: '+new Date(d.updated_at*1000).toLocaleTimeString():'Waiting for device data';
 }catch(e){$('status').classList.remove('connected');$('statusText').textContent='Dashboard API unavailable';$('connection').textContent='Offline';$('count').textContent='0';$('detections').textContent='0';$('source').textContent='Cannot reach the local API.'}
}
refresh();setInterval(refresh,2000);
</script></body></html>"""


if __name__ == "__main__":
    import uvicorn
    uvicorn.run("app:app", host=os.getenv("COLIBRY_DASHBOARD_HOST", "127.0.0.1"),
                port=int(os.getenv("COLIBRY_DASHBOARD_PORT", "8000")), reload=False)
