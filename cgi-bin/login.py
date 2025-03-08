#!/usr/bin/python3
import os
import cgi
import http.cookies
import uuid

# Configuration
LOGIN_PAGE = "/auth/login.html"
HOME_PAGE = "/home.py"
COOKIE_EXPIRE = 3600  # 1 hour in seconds

form = cgi.FieldStorage()

# Check for existing session cookie
cookie = http.cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ['HTTP_COOKIE'])

# Handle logout action
if form.getvalue('action') == 'logout':
    print("Set-Cookie: session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/")
    print("Content-Type: text/html\n")
    print(f"<script>window.location.href = '{LOGIN_PAGE}';</script>")
    exit()

# Process POST request
if os.environ["REQUEST_METHOD"] == "POST":
    username = form.getvalue("username")
    password = form.getvalue("password")

    # Dummy authentication
    if username == "zaki" and password == "123":
        session_id = str(uuid.uuid4())
        print("Set-Cookie: session_id=" + session_id)
        print("Content-Type: text/html\n")
        print(f"""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Login Successful</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    background-color: #f4f4f4;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }}
                .loading-container {{
                    text-align: center;
                }}
                .loading-spinner {{
                    border: 8px solid #f3f3f3;
                    border-top: 8px solid #3498db;
                    border-radius: 50%;
                    width: 60px;
                    height: 60px;
                    animation: spin 1s linear infinite;
                    margin: 0 auto;
                }}
                @keyframes spin {{
                    0% {{ transform: rotate(0deg); }}
                    100% {{ transform: rotate(360deg); }}
                }}
                .loading-text {{
                    margin-top: 20px;
                    font-size: 1.2em;
                    color: #333;
                }}
            </style>
        </head>
        <body>
            <div class="loading-container">
                <div class="loading-spinner"></div>
                <div class="loading-text">Login Successful! Redirecting...</div>
            </div>
            <script>
                setTimeout(() => window.location = '{HOME_PAGE}', 2000);
            </script>
        </body>
        </html>
        """)
    else:
        # Login failed: Show error message and link to login.html
        print("Content-Type: text/html\n")
        print(f"""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Login Failed</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    background-color: #f4f4f4;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }}
                .error-container {{
                    background-color: #fff;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                    width: 300px;
                    text-align: center;
                }}
                .error-message {{
                    color: #ff6b6b;
                    margin-bottom: 15px;
                }}
                .login-link {{
                    color: #3498db;
                    text-decoration: none;
                }}
                .login-link:hover {{
                    text-decoration: underline;
                }}
            </style>
        </head>
        <body>
            <div class="error-container">
                <h1>Login Failed</h1>
                <div class="error-message">Invalid username or password.</div>
                <p>Please try again at the <a class="login-link" href="{LOGIN_PAGE}">Login Page</a>.</p>
            </div>
        </body>
        </html>
        """)
else:
    # Show login form if not POST
    print("Content-Type: text/html\n")
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Login</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #f4f4f4;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                margin: 0;
            }}
            .login-container {{
                background-color: #fff;
                padding: 20px;
                border-radius: 8px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                width: 300px;
                text-align: center;
            }}
            input[type="text"], input[type="password"] {{
                width: 100%;
                padding: 10px;
                margin: 10px 0;
                border: 1px solid #ccc;
                border-radius: 4px;
            }}
            input[type="submit"] {{
                background-color: #3498db;
                color: white;
                padding: 10px 20px;
                border: none;
                border-radius: 4px;
                cursor: pointer;
            }}
            input[type="submit"]:hover {{
                background-color: #2980b9;
            }}
        </style>
    </head>
    <body>
        <div class="login-container">
            <h1>Login</h1>
            <form method="POST">
                <label for="username">Username:</label>
                <input type="text" id="username" name="username" required><br>
                <label for="password">Password:</label>
                <input type="password" id="password" name="password" required><br>
                <input type="submit" value="Login">
            </form>
        </div>
    </body>
    </html>
    """)