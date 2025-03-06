#!/usr/bin/env python3

from PIL import Image, ImageDraw
import sys
import io

# Create an image (100x100 pixels, RGB mode)
img = Image.new("RGB", (200, 100), (255, 255, 255))
draw = ImageDraw.Draw(img)

# Draw a red rectangle
draw.rectangle([10, 10, 190, 90], outline="red", width=3)

# Draw text
draw.text((50, 40), "Hello CGI!", fill="blue")

# Save to a buffer
buffer = io.BytesIO()
img.save(buffer, format="PNG")
image_data = buffer.getvalue()

# Send HTTP response headers
sys.stdout.buffer.write(b"Content-Type: image/png\n")
sys.stdout.buffer.write(b"\n")

# Send image data
sys.stdout.buffer.write(image_data)

