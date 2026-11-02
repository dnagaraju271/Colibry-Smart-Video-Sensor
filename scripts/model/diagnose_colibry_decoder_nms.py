#!/usr/bin/env python3
"""Host-only decoder/NMS sweep for a saved Colibry person detector."""
from __future__ import annotations
import argparse, json
from pathlib import Path
import numpy as np
import tensorflow as tf
import tf_keras as keras
import train_colibry_person_detector as detector

def decode(raw: np.ndarray, score_floor: float, nms_iou: float, max_boxes: int):
    rows, cols = np.meshgrid(
        np.arange(detector.GRID_ROWS, dtype=np.float32),
        np.arange(detector.GRID_COLS, dtype=np.float32), indexing="ij"
    )
    vals=1.0/(1.0+np.exp(-np.clip(raw[...,1:5],-30,30)))
    scores=1.0/(1.0+np.exp(-np.clip(raw[...,0],-30,30)))
    cx=(cols+vals[...,0])/detector.GRID_COLS
    cy=(rows+vals[...,1])/detector.GRID_ROWS
    w,h=vals[...,2],vals[...,3]
    boxes=np.stack([np.clip(cx-w/2,0,1),np.clip(cy-h/2,0,1),
                    np.clip(cx+w/2,0,1),np.clip(cy+h/2,0,1)],axis=-1).reshape(-1,4)
    scores=scores.reshape(-1)
    valid=scores>=score_floor
    boxes,scores=boxes[valid],scores[valid]
    if not len(scores): return []
    selected=tf.image.non_max_suppression(
        boxes[:,[1,0,3,2]],scores,max_output_size=max_boxes,
        iou_threshold=nms_iou,score_threshold=score_floor
    ).numpy()
    return [(boxes[i],float(scores[i])) for i in selected]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dataset",type=Path,required=True)
    p.add_argument("--model",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--thresholds",type=float,nargs="+",default=[0.25,0.35,0.5,0.7,0.9])
    p.add_argument("--nms-ious",type=float,nargs="+",default=[0.3,0.5,0.7])
    p.add_argument("--max-boxes",type=int,default=100)
    a=p.parse_args()
    out=a.output.expanduser().resolve()
    if out.exists(): p.error(f"Output already exists; choose a new path: {out}")
    x,_,gt,summary=detector.load_split(a.dataset.expanduser().resolve(),"val")
    model=keras.models.load_model(a.model.expanduser().resolve(),compile=False)
    raw=model.predict(x,batch_size=8,verbose=1)
    report={"purpose":"decoder/NMS sensitivity diagnostic on validation split; not COCO mAP",
            "model":str(a.model.expanduser().resolve()),"validation":summary,"sweeps":[]}
    for nms_iou in a.nms_ious:
        predictions=[decode(item,0.01,float(nms_iou),a.max_boxes) for item in raw]
        for threshold in a.thresholds:
            metrics=detector.evaluate_predictions(predictions,gt,float(threshold))
            counts=[sum(score>=threshold for _,score in pred) for pred in predictions]
            metrics["nms_iou_threshold"]=float(nms_iou)
            metrics["max_boxes_per_image"]=a.max_boxes
            metrics["predictions_per_image_mean"]=float(np.mean(counts))
            metrics["predictions_per_image_median"]=float(np.median(counts))
            metrics["predictions_per_image_max"]=int(max(counts,default=0))
            report["sweeps"].append(metrics)
    all_w=1.0/(1.0+np.exp(-np.clip(raw[...,3],-30,30)))
    all_h=1.0/(1.0+np.exp(-np.clip(raw[...,4],-30,30)))
    report["decoded_box_dimension_stats_normalized"]={
       "width":{"median":float(np.median(all_w)),"p90":float(np.quantile(all_w,.9)),"max":float(np.max(all_w))},
       "height":{"median":float(np.median(all_h)),"p90":float(np.quantile(all_h,.9)),"max":float(np.max(all_h))}
    }
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(report,indent=2))
    print(f"Saved report: {out}")
if __name__=="__main__": main()
