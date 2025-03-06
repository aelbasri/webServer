#!/usr/bin/env python3

# Import necessary modules
import cgi
import cgitb

# Enable CGI traceback for debugging
cgitb.enable()

# Set the content type to HTML
print("Content-Type: text/html\n")

# HTML content with CSS animation
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome to My Home Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f8ff;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }

        .welcome-message {
            font-size: 2.5em;
            color: #333;
            text-align: center;
            animation: fadeIn 2s ease-in-out, bounce 2s infinite;
        }

        @keyframes fadeIn {
            from {
                opacity: 0;
            }
            to {
                opacity: 1;
            }
        }

        @keyframes bounce {
            0%, 20%, 50%, 80%, 100% {
                transform: translateY(0);
            }
            40% {
                transform: translateY(-30px);
            }
            60% {
                transform: translateY(-15px);
            }
        }
    </style>
</head>
<body>
    <div class="welcome-message">
        Welcome to My Home Page! 🎉
    </div>
</body>
</html>
""")