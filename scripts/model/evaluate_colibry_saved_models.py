#!/usr/bin/env python3
"""Evaluate saved detector models on the untouched validation split.

This is host-only evaluation. It does not train, export, or access the physical board.
"""
from __future__ import annotations
import argparse, json, sys
from pathlib import Path
import numpy as np
import tf_keras as keras
import train_colibry_person_detector as detector

def main() -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dataset",type=Path,required=True)
    p.add_argument("--model",type=Path,action="append",required=True,
                   help="Repeat --model to evaluate multiple saved .keras models")
    p.add_argument("--output",type=Path,required=True)
    p.add_argument("--batch-size",type=int,default=8)
    p.add_argument("--thresholds",type=float,nargs="+",default=[0.25,0.35,0.5,0.7,0.9])
    a=p.parse_args()
    data=a.dataset.expanduser().resolve()
    out=a.output.expanduser().resolve()
    if out.exists():
        p.error(f"Output file already exists; choose a new path to preserve reports: {out}")
    x, y, gt_by_image, split_summary=detector.load_split(data,"val")
    results=[]
    for model_path in a.model:
        model_path=model_path.expanduser().resolve()
        model=keras.models.load_model(model_path,compile=False)
        raw=model.predict(x,batch_size=a.batch_size,verbose=1)
        predictions=[detector.decode_predictions(item,score_floor=0.01) for item in raw]
        thresholds={}
        for t in a.thresholds:
            thresholds[f"confidence_{t:.2f}"]=detector.evaluate_predictions(predictions,gt_by_image,float(t))
        results.append({
            "model":str(model_path),
            "parameters":int(model.count_params()),
            "validation_metrics":thresholds,
        })
    report={
      "purpose":"untouched validation split comparison; not COCO mAP",
      "dataset":str(data),
      "split_summary":split_summary,
      "matching":"Uses the training script's existing decode_predictions and evaluate_predictions; IoU and NMS settings are those defined there.",
      "models":results,
      "note":"Select a model only from independent validation results. This host evaluation does not establish compiler compatibility or physical-board performance."
    }
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(report,indent=2))
    print(f"Saved validation report: {out}")
    return 0
if __name__=="__main__": raise SystemExit(main())
