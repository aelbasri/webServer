#!/usr/bin/python3
import os
import cgi
import http.cookies
import uuid

# Parse query string or form data
form = cgi.FieldStorage()

# Check for existing session cookie
cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
session_cookie = cookies.get("session_id")

if session_cookie:
    # Redirect to home.html if session exists
    print("Content-Type: text/html")
    print("Location: home.html\n")

# Handle POST login
if os.environ["REQUEST_METHOD"] == "POST":
    username = form.getvalue("username")
    password = form.getvalue("password")

    # Simple authentication (replace with secure validation)
    if username == "zaki" and password == "123":
        # Create session ID and set cookie
        session_id = str(uuid.uuid4())
        print("Set-Cookie: session_id={}; Path=/".format(session_id))
        
        # Redirect to home.html after login
        print("Content-Type: text/html")
        print("Location: home.html\n")
    else:
        # Show login error
        # print("Content-Type: text/html\n")
        print("<h1>Login Failed</h1>")
        print("<p>Invalid credentials. <a href='/authentification/login.html'>Try again</a></p>")

# Show login form for GET requests
# else:
#     print("Content-Type: text/html\n")
#     print("""
#     <!DOCTYPE html>
#     <html>
#     <head>
#         <title>Login</title>
#         <style>
#             body { font-family: Arial, sans-serif; margin: 2rem; }
#             form { max-width: 300px; margin: 0 auto; }
#             input { margin-bottom: 1rem; padding: 5px; width: 100%; }
#         </style>
#     </head>
#     <body>
#         <h1>Login</h1>
#         <form method="POST">
#             <input type="text" name="username" placeholder="Username" required><br>
#             <input type="password" name="password" placeholder="Password" required><br>
#             <button type="submit">Sign in</button>
#         </form>
#     </body>
#     </html>
#     """)
