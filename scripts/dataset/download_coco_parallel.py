import csv
import time
import urllib.request
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from urllib.parse import urlparse

import cv2

MANIFEST = Path("dataset/coco_person_1000/manifest.csv")
RAW = Path("dataset/coco_person_1000/processed/raw_downloads")
WORKERS = 8
RETRIES = 3

RAW.mkdir(parents=True, exist_ok=True)

with MANIFEST.open(newline="", encoding="utf-8") as f:
    rows = list(csv.DictReader(f))

def download(row):
    filename = row["file_name"]
    dest = RAW / filename

    # Reuse an existing valid image.
    if dest.exists() and cv2.imread(str(dest)) is not None:
        return filename, "already valid"

    url = row["coco_url"].strip()
    if not url:
        return filename, "FAILED: missing URL"

    parsed = urlparse(url)
    urls = [url]
    if parsed.scheme == "http":
        urls.insert(0, parsed._replace(scheme="https").geturl())

    last_error = "unknown error"

    for candidate in urls:
        for attempt in range(RETRIES):
            temp = dest.with_suffix(".part")
            try:
                request = urllib.request.Request(
                    candidate,
                    headers={"User-Agent": "ColibryDatasetPreparation/1.0"},
                )
                with urllib.request.urlopen(request, timeout=30) as response:
                    data = response.read()

                if not data:
                    raise ValueError("Empty response")

                temp.write_bytes(data)
                if cv2.imread(str(temp)) is None:
                    raise ValueError("Response is not a valid image")

                temp.replace(dest)
                return filename, "downloaded"

            except Exception as exc:
                last_error = str(exc)
                temp.unlink(missing_ok=True)
                time.sleep(attempt + 1)

    return filename, f"FAILED: {last_error}"

# Keep the number of active requests bounded.
with ThreadPoolExecutor(max_workers=WORKERS) as pool:
    futures = [pool.submit(download, row) for row in rows]
    completed = 0
    failed = []

    for future in as_completed(futures):
        filename, status = future.result()
        completed += 1

        if status.startswith("FAILED:"):
            failed.append((filename, status))

        if completed % 25 == 0 or completed == len(rows):
            print(f"Progress: {completed}/{len(rows)}; failures: {len(failed)}")

failure_file = RAW.parent / "download_failures.csv"
with failure_file.open("w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(["file_name", "error"])
    writer.writerows(failed)

print("\nDownload pass finished.")
print(f"Manifest entries: {len(rows)}")
print(f"Failures: {len(failed)}")
print(f"Failure report: {failure_file}")
