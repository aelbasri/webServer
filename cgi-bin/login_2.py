#!/usr/bin/python3
import os
import cgi
import http.cookies
import uuid

# Parse query string or form data
form = cgi.FieldStorage()

# Check if the user is submitting a login form
if os.environ["REQUEST_METHOD"] == "POST":
    username = form.getvalue("username")
    password = form.getvalue("password")

    # Dummy authentication (replace with a real authentication mechanism)
    if username == "admin" and password == "password":
        # Create a session ID
        session_id = str(uuid.uuid4())

        # Set a cookie with the session ID
        print("Set-Cookie: session_id=" + session_id)
        print("Content-Type: text/html\n")
        print("<h1>Login Successful</h1>")
        print("<p>Welcome, " + username + "!</p>")
    else:
        print("Content-Type: text/html\n")
        print("<h1>Login Failed</h1>")
        print("<p>Invalid username or password.</p>")
else:
    # Check for an existing session cookie
    cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
    session_id = cookies.get("session_id")

    if session_id:
        print("Content-Type: text/html\n")
        print("<h1>Welcome Back</h1>")
        print("<p>Your session ID is: " + session_id.value + "</p>")
    else:
        # Display the login form
        print("Content-Type: text/html\n")
        print("<h1>Login</h1>")
        print('<form method="POST">')
        print('<label for="username">Username:</label>')
        print('<input type="text" id="username" name="username"><br>')
        print('<label for="password">Password:</label>')
        print('<input type="password" id="password" name="password"><br>')
        print('<input type="submit" value="Login">')
        print('</form>')
