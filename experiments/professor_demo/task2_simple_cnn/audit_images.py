from pathlib import Path
import matplotlib.pyplot as plt
from PIL import Image

old = Path.home() / "Colibry-Smart-Video-Sensor-git"

images_dir = (
    old / "runs/phase5j_annotation_resolution/"
    "corrected_validation/images/val"
)

output_dir = Path("dataset_audit")
output_dir.mkdir(exist_ok=True)

images = sorted(
    p for p in images_dir.iterdir()
    if p.suffix.lower() in {".png", ".jpg", ".jpeg"}
)

batch_size = 25

for start in range(0, len(images), batch_size):
    batch = images[start:start + batch_size]

    fig, axes = plt.subplots(5, 5, figsize=(15, 12))
    fig.suptitle(
        f"Image audit: {start + 1} to {start + len(batch)}",
        fontsize=14
    )

    for ax in axes.flat:
        ax.axis("off")

    for ax, path in zip(axes.flat, batch):
        with Image.open(path) as img:
            ax.imshow(img.convert("RGB"))
        ax.set_title(path.name, fontsize=7)
        ax.axis("off")

    fig.tight_layout()
    output = output_dir / f"contact_sheet_{start // batch_size + 1:02d}.jpg"
    fig.savefig(output, dpi=120)
    plt.close(fig)
    print("Created:", output)

print(f"\nTotal images: {len(images)}")
print(f"Contact sheets: {(len(images) + batch_size - 1) // batch_size}")
