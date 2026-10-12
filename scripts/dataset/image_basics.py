
import cv2
import numpy as np
from pathlib import Path

# Create a simple test image: 320 pixels wide, 240 high
image = np.zeros((240, 320, 3), dtype=np.uint8)

# Draw a rectangle representing a person
cv2.rectangle(image, (110, 30), (210, 220), (255, 255, 255), 3)

# Add a label
cv2.putText(
    image,
    "PERSON",
    (110, 22),
    cv2.FONT_HERSHEY_SIMPLEX,
    0.6,
    (255, 255, 255),
    2
)

# Convert the image to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Resize to the ColibryNPU target experiment size: 160x128
small = cv2.resize(gray, (160, 128), interpolation=cv2.INTER_AREA)

# Save the result
output = Path("tests")
output.mkdir(exist_ok=True)
cv2.imwrite(str(output / "grayscale_test.png"), small)

print("Original image shape:", image.shape)
print("Grayscale image shape:", gray.shape)
print("Resized image shape:", small.shape)
print("Saved: tests/grayscale_test.png")

