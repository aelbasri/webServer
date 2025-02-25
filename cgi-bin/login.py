#!/usr/bin/env python3

import sys
import http.cookies

# Hardcoded username and password for demonstration purposes
VALID_USERNAME = "user"
VALID_PASSWORD = "pass"

# Function to set a cookie
def set_cookie(username):
    cookie = http.cookies.SimpleCookie()
    cookie["username"] = username
    cookie["username"]["max-age"] = 3600  # Cookie expires in 1 hour
    print(cookie.output())

# Function to parse the request body
def parse_body(body):
    data = {}
    for pair in body.split("&"):
        if "=" in pair:
            key, value = pair.split("=", 1)  # Split on the first '=' only
            data[key] = value
        else:
            # Handle malformed pairs (e.g., missing '=')
            data[pair] = ""  # Assign an empty value
    return data

# Main function
def main():
    # Read the request body from stdin
    body = sys.stdin.read()

    # Parse the request body
    form_data = parse_body(body)

    # Extract form fields
    username = form_data.get("username", "")
    password = form_data.get("password", "")
    remember_me = form_data.get("remember_me", "")

    # Debug: Print the parsed data
    print("Content-Type: text/plain\n")  # Temporarily use text/plain for debugging
    print(f"Username: {username}")
    print(f"Password: {password}")
    print(f"Remember Me: {remember_me}")

    # Validate the username and password
    if username == VALID_USERNAME and password == VALID_PASSWORD:
        print("Content-Type: text/html\n")  # Switch back to HTML for the response
        print("<h1>Login Successful</h1>")
        print(f"<p>Welcome, {username}!</p>")

        # Set a cookie if "Remember Me" is checked
        if remember_me == "on":
            set_cookie(username)
            print("<p>A cookie has been set to remember you.</p>")
    else:
        print("Content-Type: text/html\n")
        print("<h1>Login Failed</h1>")
        print("<p>Invalid username or password.</p>")
        print('<p><a href="/login.html">Try again</a></p>')

if __name__ == "__main__":
    main()