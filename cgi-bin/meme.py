#!/usr/bin/env python3

import sys
import requests
import io

# Meme API (replace with any image URL)
meme_url = "https://i.imgflip.com/4/1bij.jpg"  # Example meme (change as needed)

try:
    # Fetch the image
    response = requests.get(meme_url)
    response.raise_for_status()  # Raise error if request fails
    image_data = response.content

    # Send HTTP headers
    sys.stdout.buffer.write(b"Content-Type: image/jpeg\n")
    sys.stdout.buffer.write(b"\n")

    # Send image content
    sys.stdout.buffer.write(image_data)

except Exception as e:
    sys.stdout.buffer.write(b"Content-Type: text/plain\n\n")
    sys.stdout.buffer.write(f"Error fetching meme: {str(e)}".encode())

