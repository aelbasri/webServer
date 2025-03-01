#!/bin/bash

# Read input from stdin
read input_data

# Parse the input (e.g., name=John)
IFS='&' read -r -a pairs <<< "$input_data"
for pair in "${pairs[@]}"; do
    IFS='=' read -r key value <<< "$pair"
    if [[ "$key" == "name" ]]; then
        name="$value"
    fi
done

# Default to "Guest" if name is not provided
name="${name:-Guest}"

# Print the response
echo "Content-Type: text/html"
echo
echo "<h1>Hello, $name!</h1>"
echo "<p>This is a Bash CGI script.</p>"