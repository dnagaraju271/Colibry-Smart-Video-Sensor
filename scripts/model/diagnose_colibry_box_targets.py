#!/usr/bin/env python3
"""Inspect ground-truth box targets versus raw model box outputs on validation data."""
from __future__ import annotations
import argparse, json
from pathlib import Path
import numpy as np
import tf_keras as keras
import train_colibry_person_detector as detector

def stats(a):
    a=np.asarray(a,dtype=np.float64).reshape(-1)
    return {"count":int(a.size),"min":float(a.min()),"median":float(np.median(a)),
            "p90":float(np.quantile(a,.9)),"max":float(a.max()),"mean":float(a.mean())}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dataset",type=Path,required=True)
    p.add_argument("--model",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True)
    a=p.parse_args()
    out=a.output.expanduser().resolve()
    if out.exists(): p.error(f"Output already exists; choose a new path: {out}")
    x,y,gt,summary=detector.load_split(a.dataset.expanduser().resolve(),"val")
    model=keras.models.load_model(a.model.expanduser().resolve(),compile=False)
    raw=model.predict(x,batch_size=8,verbose=1)
    target_pos=y[...,0]>.5
    pred_sigmoid=1/(1+np.exp(-np.clip(raw[...,1:5],-30,30)))
    true_pos=y[...,1:5][target_pos]
    pred_pos=pred_sigmoid[target_pos]
    # For every labelled positive cell, decode GT and predicted xyxy in normalized image coords.
    rr,cc=np.where(target_pos[0]) if False else (None,None)
    rows=np.broadcast_to(np.arange(detector.GRID_ROWS)[None,:,None],target_pos.shape)
    cols=np.broadcast_to(np.arange(detector.GRID_COLS)[None,None,:],target_pos.shape)
    t=y[...,1:5]
    tcx=(cols+t[...,0])/detector.GRID_COLS
    tcy=(rows+t[...,1])/detector.GRID_ROWS
    pcx=(cols+pred_sigmoid[...,0])/detector.GRID_COLS
    pcy=(rows+pred_sigmoid[...,1])/detector.GRID_ROWS
    tw,th=t[...,2],t[...,3]
    pw,ph=pred_sigmoid[...,2],pred_sigmoid[...,3]
    tbox=np.stack([tcx-tw/2,tcy-th/2,tcx+tw/2,tcy+th/2],axis=-1)[target_pos]
    pbox=np.stack([pcx-pw/2,pcy-ph/2,pcx+pw/2,pcy+ph/2],axis=-1)[target_pos]
    def iou_pairs(a,b):
        ix=np.maximum(np.minimum(a[:,2],b[:,2])-np.maximum(a[:,0],b[:,0]),0)
        iy=np.maximum(np.minimum(a[:,3],b[:,3])-np.maximum(a[:,1],b[:,1]),0)
        inter=ix*iy
        aa=np.maximum(a[:,2]-a[:,0],0)*np.maximum(a[:,3]-a[:,1],0)
        ba=np.maximum(b[:,2]-b[:,0],0)*np.maximum(b[:,3]-b[:,1],0)
        return inter/(aa+ba-inter+1e-9)
    report={
      "purpose":"box target/output diagnostic; validation labels only; not COCO mAP",
      "model":str(a.model.expanduser().resolve()),"validation":summary,
      "positive_cells":int(target_pos.sum()),
      "ground_truth_cell_width":stats(true_pos[:,2]),
      "ground_truth_cell_height":stats(true_pos[:,3]),
      "predicted_cell_width_on_positive_cells":stats(pred_pos[:,2]),
      "predicted_cell_height_on_positive_cells":stats(pred_pos[:,3]),
      "target_center_offsets_x":stats(true_pos[:,0]),
      "target_center_offsets_y":stats(true_pos[:,1]),
      "predicted_center_offsets_x":stats(pred_pos[:,0]),
      "predicted_center_offsets_y":stats(pred_pos[:,1]),
      "positive_cell_box_iou":stats(iou_pairs(tbox,pbox)),
      "note":"Predictions are sampled at target-positive cells to isolate box regression from objectness ranking. This is diagnostic only."
    }
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(report,indent=2)); print(f"Saved report: {out}")
if __name__=="__main__": main()
