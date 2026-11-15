from pathlib import Path
import argparse

import matplotlib.pyplot as plt
from PIL import Image


def main():
    parser = argparse.ArgumentParser(
        description="Generate 5x5 image-audit contact sheets."
    )
    parser.add_argument(
        "--images-dir",
        type=Path,
        required=True,
        help="Directory containing the validation images.",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path(__file__).resolve().parent / "dataset_audit",
    )
    args = parser.parse_args()

    images_dir = args.images_dir.expanduser().resolve()
    output_dir = args.output_dir.expanduser().resolve()

    if not images_dir.is_dir():
        parser.error(f"Image directory does not exist: {images_dir}")

    images = sorted(
        p for p in images_dir.iterdir()
        if p.suffix.lower() in {".png", ".jpg", ".jpeg"}
    )

    if not images:
        parser.error(f"No supported images found in: {images_dir}")

    output_dir.mkdir(parents=True, exist_ok=True)
    batch_size = 25

    for start in range(0, len(images), batch_size):
        batch = images[start:start + batch_size]
        fig, axes = plt.subplots(5, 5, figsize=(15, 12))
        fig.suptitle(
            f"Image audit: {start + 1} to {start + len(batch)}",
            fontsize=14,
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


if __name__ == "__main__":
    main()
