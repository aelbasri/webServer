#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Save this as upload_handler.py in your cgi-bin directory
# Make sure to set executable permissions: chmod +x upload_handler.py

import cgi
import cgitb
import os
import sys
import json
import tempfile
import requests
from urllib.parse import parse_qs

# Enable CGI error traceback for debugging
cgitb.enable()

def main():
    # Print required CGI response header
    print("Content-Type: application/json")
    print()  # Empty line is required after headers
    
    # Response data structure
    response = {
        "success": False,
        "message": "",
        "error": ""
    }
    
    try:
        # Get the form data from the request
        form = cgi.FieldStorage()
        
        # Check if we have a file
        if 'file' not in form:
            response["error"] = "No file was uploaded"
            print(json.dumps(response))
            return
        
        # Get the uploaded file
        file_item = form['file']
        
        # Check if it's a proper file upload
        if not file_item.file:
            response["error"] = "Invalid file upload"
            print(json.dumps(response))
            return
        
        # Get the destination URL to forward the upload to
        # You can set a default or get it from the form
        destination_url = form.getvalue('destination_url', 'http://localhost:3000/')
        
        # Should we use chunked encoding?
        use_chunked = form.getvalue('use_chunked_encoding', 'true').lower() == 'true'
        
        # Get the file data
        filename = os.path.basename(file_item.filename)
        
        # Create a temporary file to store the uploaded data
        with tempfile.NamedTemporaryFile(delete=False) as temp_file:
            # Read the uploaded file and write to temp file
            while True:
                chunk = file_item.file.read(64 * 1024)  # Read 64KB at a time
                if not chunk:
                    break
                temp_file.write(chunk)
            
            temp_file_path = temp_file.name
        
        # Now forward the file using requests with chunked encoding
        headers = {
            'Content-Type': 'application/octet-stream',
            'X-File-Name': filename
        }
        
        # Create a generator to stream the file in chunks
        def file_chunk_generator(file_path, chunk_size=64*1024):
            with open(file_path, 'rb') as f:
                while True:
                    chunk = f.read(chunk_size)
                    if not chunk:
                        break
                    yield chunk
        
        # Send the request with chunked encoding
        requests_response = requests.post(
            destination_url,
            headers=headers,
            data=file_chunk_generator(temp_file_path) if use_chunked else open(temp_file_path, 'rb'),
            # When using a generator, requests will automatically use chunked encoding
        )
        
        # Clean up the temporary file
        try:
            os.unlink(temp_file_path)
        except:
            pass
        
        # Check the response
        if requests_response.status_code >= 200 and requests_response.status_code < 300:
            response["success"] = True
            response["message"] = f"File '{filename}' was uploaded successfully using {'chunked' if use_chunked else 'standard'} encoding"
        else:
            response["error"] = f"Server returned status code {requests_response.status_code}: {requests_response.text}"
    
    except Exception as e:
        # Capture any errors
        response["error"] = str(e)
    
    # Return the JSON response
    print(json.dumps(response))

if __name__ == "__main__":
    main()