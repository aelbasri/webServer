#!/usr/bin/env python3

import sys

# Read input from stdin
input_data = sys.stdin.read()

# Parse the input (e.g., name=John)
data = dict(pair.split('=') for pair in input_data.split('&'))
name = data.get('name', 'Guest')

# Print the response
print("Content-Type: text/html\n")
print(f"<h1>Hello, {name}!</h1>")
print("<p>This is a Python CGI script.</p>")