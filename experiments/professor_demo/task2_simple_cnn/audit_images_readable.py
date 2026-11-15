from pathlib import Path
from PIL import Image
import matplotlib.pyplot as plt
import math

old = Path.home() / "Colibry-Smart-Video-Sensor-git"
images_dir = old / "runs/phase5j_annotation_resolution/corrected_validation/images/val"
output_dir = Path("dataset_audit_readable")
output_dir.mkdir(exist_ok=True)

images = sorted(
    p for p in images_dir.iterdir()
    if p.suffix.lower() in {".png", ".jpg", ".jpeg"}
)

batch_size = 10

for start in range(0, len(images), batch_size):
    batch = images[start:start + batch_size]
    fig, axes = plt.subplots(2, 5, figsize=(20, 10))
    fig.suptitle(f"Images {start + 1} to {start + len(batch)}", fontsize=18)

    for ax in axes.flat:
        ax.axis("off")

    for ax, path in zip(axes.flat, batch):
        with Image.open(path) as im:
            ax.imshow(im.convert("RGB"))
        ax.set_title(path.name, fontsize=11)
        ax.axis("off")

    fig.tight_layout()
    number = start // batch_size + 1
    output = output_dir / f"contact_sheet_{number:02d}.jpg"
    fig.savefig(output, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(output)

print("Total images:", len(images))
