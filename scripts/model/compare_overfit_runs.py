#!/usr/bin/env python3
"""Compare two saved overfit sanity reports and print a compact metric table."""
from __future__ import annotations
import argparse, json
from pathlib import Path

def load(path: Path) -> dict:
    return json.loads(path.expanduser().read_text(encoding="utf-8"))

def main() -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--baseline", type=Path, required=True)
    p.add_argument("--candidate", type=Path, required=True)
    a=p.parse_args()
    reports=[load(a.baseline),load(a.candidate)]
    names=["baseline","candidate"]
    print("purpose: small training-subset diagnostic only; not validation performance")
    print(f"{'metric':42} {'baseline':>14} {'candidate':>14}")
    print("-"*73)
    rows=[
      ("first_training_loss","first_training_loss"),
      ("minimum_training_loss","minimum_training_loss"),
      ("last_training_loss","last_training_loss"),
    ]
    for label,key in rows:
      print(f"{label:42} {reports[0].get(key,float('nan')):14.4f} {reports[1].get(key,float('nan')):14.4f}")
    for threshold in ("0.25","0.50","0.70","0.90"):
      key=f"confidence_{threshold}"
      for metric in ("true_positives","false_positives","false_negatives","precision","recall","f1","negative_images_with_detections"):
        vals=[r.get("training_subset_metrics",{}).get(key,{}).get(metric) for r in reports]
        def fmt(v):
          return "missing" if v is None else (f"{v:.4f}" if isinstance(v,float) else str(v))
        print(f"{key}.{metric:30} {fmt(vals[0]):>14} {fmt(vals[1]):>14}")
    print("\nDo not choose a model from training-subset metrics alone; evaluate on the untouched validation split.")
    return 0
if __name__=="__main__": raise SystemExit(main())
