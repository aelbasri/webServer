


#!/usr/bin/python3
import os
import cgi
import http.cookies
import uuid

# Configuration
LOGIN_PAGE = "/authentification/login.html"
HOME_PAGE = "/cgi-bin/home.py"
COOKIE_EXPIRE = 3600  # 1 hour in seconds

form = cgi.FieldStorage()

# Check for existing session cookie
cookie = http.cookies.SimpleCookie()
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ['HTTP_COOKIE'])

# Handle logout action
if form.getvalue('action') == 'logout':
    # print("Set-Cookie: session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/")
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
        print(f"""<h1>Login Successful</h1>
               <p>Welcome {username}!</p>
               <script>setTimeout(() => window.location = '{HOME_PAGE}', 2000);</script>""")
    else:
        print("Content-Type: text/html\n")
        print(f"""<h1>Login Failed</h1>
               <p>Invalid credentials. Please try again at <a href="{LOGIN_PAGE}">Login Page</a></p>""")
else:
    # Show login form if not POST
    print("Content-Type: text/html\n")
    print(f"""<h1>Login</h1>
           <form method="POST">
               <label for="username">Username:</label>
               <input type="text" id="username" name="username"><br>
               <label for="password">Password:</label>
               <input type="password" id="password" name="password"><br>
               <input type="submit" value="Login">
           </form>
           <p>Or access the <a href="{LOGIN_PAGE}">Login Page</a></p>""")