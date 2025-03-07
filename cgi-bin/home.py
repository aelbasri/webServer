import cgi
import cgitb
import requests
import json
import time
from html import escape
from datetime import datetime

# Enable CGI traceback for debugging
cgitb.enable()

# Configuration
TEAM_MEMBERS = [
    {"name": "John Doe", "role": "C++ Developer", "github": "Khdzakariae"},
    {"name": "Jane Smith", "role": "System Architect", "github": "aelbasri"},
    {"name": "Alice Johnson", "role": "Network Engineer", "github": "saad-out"}
]

# Mock server stats (would be replaced by real metrics in production)
SERVER_STATS = {
    "uptime": "99.98%",
    "response_time": "45ms",
    "active_connections": 128,
    "requests_per_second": 256
}

def get_github_user(username):
    try:
        response = requests.get(f"https://api.github.com/users/{username}")
        return response.json()
    except:
        return {"login": username, "name": username, "avatar_url": "#", "html_url": "#"}

# Set the content type to HTML
print("Content-Type: text/html\n")

# Current date for dynamic display
current_date = datetime.now().strftime("%B %d, %Y")

print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv - C++ HTTP Server</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
    <style>
        /* Global Styles */
        :root {{
            --primary-color: #3498db;
            --secondary-color: #2c3e50;
            --accent-color: #e74c3c;
            --light-color: #ecf0f1;
            --dark-color: #2c3e50;
            --success-color: #2ecc71;
            --font-main: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }}
        
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        html {{
            scroll-behavior: smooth;
        }}
        
        body {{
            font-family: var(--font-main);
            line-height: 1.6;
            color: #333;
            background-color: #f9f9f9;
            overflow-x: hidden;
        }}
        
        .container {{
            width: 90%;
            max-width: 1200px;
            margin: 0 auto;
            padding: 0 15px;
        }}
        
        section {{
            padding: 80px 0;
        }}
        
        /* Header & Navigation */
        header {{
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            background-color: rgba(255, 255, 255, 0.95);
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
            z-index: 1000;
            transition: all 0.3s ease;
        }}
        
        header.scrolled {{
            background-color: var(--secondary-color);
        }}
        
        header.scrolled .logo,
        header.scrolled .nav-links a {{
            color: white;
        }}
        
        nav {{
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 20px 0;
        }}
        
        .logo {{
            font-size: 1.8rem;
            font-weight: 700;
            color: var(--secondary-color);
            text-decoration: none;
            display: flex;
            align-items: center;
            transition: color 0.3s ease;
        }}
        
        .logo-icon {{
            margin-right: 10px;
            color: var(--primary-color);
        }}
        
        .nav-links {{
            display: flex;
            gap: 25px;
        }}
        
        .nav-links a {{
            text-decoration: none;
            color: var(--dark-color);
            font-weight: 500;
            position: relative;
            transition: color 0.3s ease;
        }}
        
        .nav-links a:hover {{
            color: var(--primary-color);
        }}
        
        .nav-links a::after {{
            content: '';
            position: absolute;
            bottom: -5px;
            left: 0;
            width: 0;
            height: 2px;
            background-color: var(--primary-color);
            transition: width 0.3s ease;
        }}
        
        .nav-links a:hover::after {{
            width: 100%;
        }}
        
        .mobile-menu-btn {{
            display: none;
            background: none;
            border: none;
            font-size: 1.5rem;
            cursor: pointer;
            color: var(--dark-color);
        }}
        
        /* Hero Section */
        .hero {{
            height: 100vh;
            background: linear-gradient(135deg, #3498db, #2c3e50);
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            color: white;
            position: relative;
            text-align: center;
            padding-top: 60px;
            overflow: hidden;
        }}
        
        .hero::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: url('data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIxMDAlIiBoZWlnaHQ9IjEwMCUiPjxkZWZzPjxwYXR0ZXJuIGlkPSJwYXR0ZXJuXzEiIHBhdHRlcm5Vbml0cz0idXNlclNwYWNlT25Vc2UiIHdpZHRoPSIxMCIgaGVpZ2h0PSIxMCIgcGF0dGVyblRyYW5zZm9ybT0icm90YXRlKDQ1KSI+PHBhdGggZD0iTSAwIDAgTCAxMCAwIEwgMTAgMSBMIDAgMSBaIiBmaWxsPSIjZmZmZmZmMTAiLz48L3BhdHRlcm4+PC9kZWZzPjxyZWN0IHdpZHRoPSIxMDAlIiBoZWlnaHQ9IjEwMCUiIGZpbGw9InVybCgjcGF0dGVybl8xKSIvPjwvc3ZnPg==') repeat;
            opacity: 0.1;
        }}
        
        .hero-content {{
            max-width: 800px;
            padding: 0 20px;
            z-index: 1;
        }}
        
        .hero h1 {{
            font-size: 3.5rem;
            margin-bottom: 20px;
            letter-spacing: 1px;
            animation: fadeIn 1s ease-out;
        }}
        
        .hero p {{
            font-size: 1.5rem;
            margin-bottom: 40px;
            opacity: 0.9;
            animation: fadeIn 1s ease-out 0.3s forwards;
            opacity: 0;
        }}
        
        .cta-buttons {{
            display: flex;
            justify-content: center;
            gap: 20px;
            margin-top: 30px;
            animation: fadeIn 1s ease-out 0.6s forwards;
            opacity: 0;
        }}
        
        .cta-button {{
            display: inline-block;
            padding: 15px 30px;
            border-radius: 30px;
            text-decoration: none;
            font-weight: 600;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 1px;
        }}
        
        .cta-button:first-child {{
            background-color: white;
            color: var(--primary-color);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }}
        
        .cta-button:first-child:hover {{
            transform: translateY(-3px);
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
        }}
        
        .cta-button.secondary {{
            border: 2px solid white;
            color: white;
        }}
        
        .cta-button.secondary:hover {{
            background-color: white;
            color: var(--primary-color);
        }}
        
        .scroll-indicator {{
            position: absolute;
            bottom: 40px;
            animation: bounce 2s infinite;
            text-align: center;
            color: rgba(255, 255, 255, 0.7);
        }}
        
        .scroll-icon {{
            height: 30px;
            width: 20px;
            border: 2px solid rgba(255, 255, 255, 0.7);
            border-radius: 10px;
            position: relative;
            margin: 10px auto;
        }}
        
        .scroll-icon::before {{
            content: '';
            position: absolute;
            top: 5px;
            left: 50%;
            width: 4px;
            height: 4px;
            background-color: rgba(255, 255, 255, 0.7);
            border-radius: 50%;
            transform: translateX(-50%);
            animation: scrollDot 1.5s infinite;
        }}
        
        /* Features Section */
        .features {{
            background-color: #fff;
        }}
        
        .section-title {{
            text-align: center;
            margin-bottom: 60px;
        }}
        
        .section-title h2 {{
            font-size: 2.5rem;
            color: var(--dark-color);
            margin-bottom: 15px;
            position: relative;
            display: inline-block;
        }}
        
        .section-title h2::after {{
            content: '';
            position: absolute;
            bottom: -10px;
            left: 50%;
            transform: translateX(-50%);
            width: 70px;
            height: 3px;
            background-color: var(--primary-color);
        }}
        
        .section-title p {{
            font-size: 1.1rem;
            color: #777;
            max-width: 700px;
            margin: 0 auto;
        }}
        
        .features-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 30px;
        }}
        
        .feature-card {{
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
            padding: 30px;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }}
        
        .feature-card:hover {{
            transform: translateY(-10px);
            box-shadow: 0 15px 40px rgba(0, 0, 0, 0.15);
        }}
        
        .feature-icon {{
            font-size: 2.5rem;
            color: var(--primary-color);
            margin-bottom: 20px;
        }}
        
        .feature-card h3 {{
            font-size: 1.4rem;
            margin-bottom: 15px;
            color: var(--dark-color);
        }}
        
        .feature-card p {{
            color: #777;
            margin-bottom: 20px;
        }}
        
        .feature-link {{
            display: inline-block;
            text-decoration: none;
            color: var(--primary-color);
            font-weight: 600;
            transition: color 0.3s ease;
        }}
        
        .feature-link:hover {{
            color: var(--secondary-color);
        }}
        
        .feature-link i {{
            margin-left: 5px;
            transition: transform 0.3s ease;
        }}
        
        .feature-link:hover i {{
            transform: translateX(5px);
        }}
        
        /* Stats Section */
        .stats-section {{
            background: linear-gradient(135deg, #3949ab, #1e3c72);
            color: white;
            position: relative;
            overflow: hidden;
        }}
        
        .stats-section::before {{
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: url('data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIxMDAlIiBoZWlnaHQ9IjEwMCUiPjxkZWZzPjxwYXR0ZXJuIGlkPSJwYXR0ZXJuXzEiIHBhdHRlcm5Vbml0cz0idXNlclNwYWNlT25Vc2UiIHdpZHRoPSIxMCIgaGVpZ2h0PSIxMCIgcGF0dGVyblRyYW5zZm9ybT0icm90YXRlKDQ1KSI+PHBhdGggZD0iTSAwIDAgTCAxMCAwIEwgMTAgMSBMIDAgMSBaIiBmaWxsPSIjZmZmZmZmMTAiLz48L3BhdHRlcm4+PC9kZWZzPjxyZWN0IHdpZHRoPSIxMDAlIiBoZWlnaHQ9IjEwMCUiIGZpbGw9InVybCgjcGF0dGVybl8xKSIvPjwvc3ZnPg==') repeat;
            opacity: 0.1;
        }}
        
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 30px;
            text-align: center;
        }}
        
        .stat-item {{
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 30px 20px;
            transition: transform 0.3s ease;
        }}
        
        .stat-item:hover {{
            transform: translateY(-5px);
        }}
        
        .stat-value {{
            font-size: 2.5rem;
            font-weight: 700;
            margin-bottom: 10px;
        }}
        
        .stat-label {{
            font-size: 1.1rem;
            opacity: 0.8;
        }}
        
        /* Team Section */
        .team-section {{
            background-color: #f9f9f9;
        }}
        
        .team-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 30px;
        }}
        
        .team-card {{
            background-color: #fff;
            border-radius: 10px;
            overflow: hidden;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.08);
            text-align: center;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }}
        
        .team-card:hover {{
            transform: translateY(-10px);
            box-shadow: 0 15px 40px rgba(0, 0, 0, 0.15);
        }}
        
        .team-image {{
            width: 100%;
            height: 250px;
            object-fit: cover;
            transition: transform 0.5s ease;
        }}
        
        .team-card:hover .team-image {{
            transform: scale(1.05);
        }}
        
        .team-card h3 {{
            font-size: 1.4rem;
            margin: 20px 0 5px;
            color: var(--dark-color);
        }}
        
        .team-card p {{
            color: #777;
            margin-bottom: 15px;
        }}
        
        .social-icons {{
            display: flex;
            justify-content: center;
            gap: 15px;
            padding: 15px 0 25px;
        }}
        
        .social-icons a {{
            display: inline-flex;
            align-items: center;
            justify-content: center;
            width: 40px;
            height: 40px;
            border-radius: 50%;
            background-color: #f1f1f1;
            color: var(--dark-color);
            text-decoration: none;
            transition: all 0.3s ease;
        }}
        
        .social-icons a:hover {{
            background-color: var(--primary-color);
            color: white;
            transform: translateY(-3px);
        }}
        
        /* Footer */
        footer {{
            background-color: var(--dark-color);
            color: white;
            padding: 70px 0 0;
        }}
        
        .footer-content {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 40px;
            margin-bottom: 50px;
        }}
        
        .footer-column h3 {{
            font-size: 1.4rem;
            margin-bottom: 20px;
            position: relative;
            display: inline-block;
        }}
        
        .footer-column h3::after {{
            content: '';
            position: absolute;
            bottom: -8px;
            left: 0;
            width: 40px;
            height: 2px;
            background-color: var(--primary-color);
        }}
        
        .footer-column p {{
            opacity: 0.8;
            margin-bottom: 10px;
        }}
        
        .footer-links {{
            list-style: none;
        }}
        
        .footer-links li {{
            margin-bottom: 10px;
        }}
        
        .footer-links a {{
            color: rgba(255, 255, 255, 0.8);
            text-decoration: none;
            transition: color 0.3s ease;
        }}
        
        .footer-links a:hover {{
            color: var(--primary-color);
        }}
        
        .copyright {{
            text-align: center;
            padding: 20px 0;
            border-top: 1px solid rgba(255, 255, 255, 0.1);
            opacity: 0.7;
            font-size: 0.9rem;
        }}
        
        /* Animations */
        @keyframes fadeIn {{
            from {{
                opacity: 0;
                transform: translateY(20px);
            }}
            to {{
                opacity: 1;
                transform: translateY(0);
            }}
        }}
        
        @keyframes bounce {{
            0%, 20%, 50%, 80%, 100% {{
                transform: translateY(0);
            }}
            40% {{
                transform: translateY(-15px);
            }}
            60% {{
                transform: translateY(-10px);
            }}
        }}
        
        @keyframes scrollDot {{
            0% {{
                opacity: 1;
                top: 5px;
            }}
            50% {{
                opacity: 0;
                top: 15px;
            }}
            100% {{
                opacity: 1;
                top: 5px;
            }}
        }}
        
        /* Responsive Styles */
        @media (max-width: 992px) {{
            .hero h1 {{
                font-size: 2.8rem;
            }}
            
            .hero p {{
                font-size: 1.3rem;
            }}
        }}
        
        @media (max-width: 768px) {{
            .mobile-menu-btn {{
                display: block;
            }}
            
            .nav-links {{
                position: fixed;
                top: 70px;
                left: 0;
                right: 0;
                background-color: white;
                flex-direction: column;
                padding: 20px;
                box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
                clip-path: circle(0px at top right);
                transition: all 0.4s ease-out;
                pointer-events: none;
            }}
            
            .nav-links.active {{
                clip-path: circle(1500px at top right);
                pointer-events: all;
            }}
            
            .hero h1 {{
                font-size: 2.3rem;
            }}
            
            .hero p {{
                font-size: 1.1rem;
            }}
            
            .cta-buttons {{
                flex-direction: column;
                gap: 15px;
            }}
            
            .section-title h2 {{
                font-size: 2rem;
            }}
        }}
        
        /* Live Date Display */
        .live-date {{
            position: absolute;
            top: 15px;
            right: 15px;
            font-size: 0.9rem;
            opacity: 0.7;
        }}
        
        /* Loading Spinner */
        .loading-spinner {{
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: #fff;
            display: flex;
            justify-content: center;
            align-items: center;
            z-index: 9999;
            transition: opacity 0.5s ease, visibility 0.5s ease;
        }}
        
        .spinner {{
            width: 60px;
            height: 60px;
            border: 5px solid #f3f3f3;
            border-top: 5px solid var(--primary-color);
            border-radius: 50%;
            animation: spin 1s linear infinite;
        }}
        
        @keyframes spin {{
            0% {{ transform: rotate(0deg); }}
            100% {{ transform: rotate(360deg); }}
        }}
    </style>
</head>
<body>
    <!-- Loading Spinner -->
    <div class="loading-spinner" id="loadingSpinner">
        <div class="spinner"></div>
    </div>

    <header id="header">
        <div class="container">
            <nav>
                <a href="#" class="logo">
                    <i class="fas fa-server logo-icon"></i>
                    Webserv
                </a>
                <button class="mobile-menu-btn" id="mobileMenuBtn">
                    <i class="fas fa-bars"></i>
                </button>
                <div class="nav-links" id="navLinks">
                    <a href="#home">Home</a>
                    <a href="#features">Features</a>
                    <a href="#stats">Stats</a>
                    <a href="#team">Team</a>
                    <a href="#about">About</a>
                    <a href="#contact">Contact</a>
                </div>
            </nav>
        </div>
    </header>

    <section class="hero" id="home">
        <div class="live-date">{current_date}</div>
        <div class="hero-content">
            <h1>Welcome to Webserv</h1>
            <p>A high-performance HTTP server written in C++ 98</p>
            <div class="cta-buttons">
                <a href="#features" class="cta-button">Explore Features</a>
                <a href="#about" class="cta-button secondary">Learn More</a>
            </div>
        </div>
        <div class="scroll-indicator">
            <p>Scroll down</p>
            <div class="scroll-icon"></div>
        </div>
    </section>

    <section class="features" id="features">
        <div class="container">
            <div class="section-title">
                <h2>Core Features</h2>
                <p>Webserv comes with a powerful set of features designed for modern web applications</p>
            </div>
            <div class="features-grid">
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-bolt"></i>
                    </div>
                    <h3>Non-blocking I/O</h3>
                    <p>Handle multiple connections simultaneously with non-blocking I/O operations, ensuring high performance and scalability.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-shield-alt"></i>
                    </div>
                    <h3>Security</h3>
                    <p>Built-in protection against common web vulnerabilities, with support for secure communication protocols.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-rocket"></i>
                    </div>
                    <h3>HTTP/1.1 Compliant</h3>
                    <p>Fully compliant with HTTP/1.1 standards, supporting persistent connections, chunked transfer encoding, and more.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-code"></i>
                    </div>
                    <h3>CGI Support</h3>
                    <p>Support for CGI scripts, allowing dynamic content generation with languages like PHP, Python, and more.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-paint-brush"></i>
                    </div>
                    <h3>Configuration File</h3>
                    <p>Customizable server behavior through a configuration file, including port settings, error pages, and routing rules.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
                <div class="feature-card">
                    <div class="feature-icon">
                        <i class="fas fa-database"></i>
                    </div>
                    <h3>File Uploads</h3>
                    <p>Support for file uploads via POST requests, with configurable storage locations and size limits.</p>
                    <a href="#" class="feature-link">Learn more <i class="fas fa-arrow-right"></i></a>
                </div>
            </div>
        </div>
    </section>
    
    <section class="stats-section" id="stats">
        <div class="container">
            <div class="section-title">
                <h2 style="color: white;">Performance Metrics</h2>
                <p style="color: rgba(255,255,255,0.8);">Our server metrics showcase why Webserv is the right choice for your application</p>
            </div>
            <div class="stats-grid">
                <div class="stat-item">
                    <div class="stat-value" data-value="{SERVER_STATS['uptime']}">0</div>
                    <div class="stat-label">Uptime</div>
                </div>
                <div class="stat-item">
                    <div class="stat-value" data-value="{SERVER_STATS['response_time']}">0</div>
                    <div class="stat-label">Avg. Response Time</div>
                </div>
                <div class="stat-item">
                    <div class="stat-value" data-value="{SERVER_STATS['active_connections']}">0</div>
                    <div class="stat-label">Active Connections</div>
                </div>
                <div class="stat-item">
                    <div class="stat-value" data-value="{SERVER_STATS['requests_per_second']}">0</div>
                    <div class="stat-label">Requests per Second</div>
                </div>
            </div>
        </div>
    </section>

    <section class="team-section" id="team">
        <div class="container">
            <div class="section-title">
                <h2>Development Team</h2>
                <p>Meet the talented individuals behind Webserv</p>
            </div>
            <div class="team-grid">
""")

# Dynamic team section
for member in TEAM_MEMBERS:
    gh_data = get_github_user(member["github"])
    print(f"""
                <div class="team-card">
                    <img src="{gh_data.get('avatar_url', '#')}" alt="{gh_data.get('name', gh_data.get('login', member['name']))}" class="team-image">
                    <h3>{gh_data.get('name', gh_data.get('login', member['name']))}</h3>
                    <p>{member['role']}</p>
                    <div class="social-icons">
                        <a href="{gh_data.get('html_url', '#')}" target="_blank"><i class="fab fa-github"></i></a>
                        <a href="#" target="_blank"><i class="fab fa-twitter"></i></a>
                        <a href="#" target="_blank"><i class="fab fa-linkedin"></i></a>
                    </div>
                </div>
    """)

print("""
            </div>
        </div>
    </section>

    <!-- About Section -->
    <section id="about" class="about-section">
        <div class="container">
            <div class="section-title">
                <h2>About Webserv</h2>
<p>Learn more about our project and its development journey</p>
            </div>
            <div class="about-content">
                <div class="about-text">
                    <h3>Our Story</h3>
                    <p>Webserv started as an educational project focused on building a high-performance HTTP server from scratch using only C++ 98 standards. What began as a learning experience evolved into a robust server solution capable of handling real-world web applications.</p>
                    
                    <h3>Our Mission</h3>
                    <p>We aim to provide a lightweight, efficient, and standards-compliant HTTP server that offers developers full control over their web infrastructure while maintaining excellent performance and security.</p>
                    
                    <div class="tech-stack">
                        <h3>Technology Stack</h3>
                        <div class="tech-icons">
                            <div class="tech-icon">
                                <i class="fab fa-cpp"></i>
                                <span>C++</span>
                            </div>
                            <div class="tech-icon">
                                <i class="fas fa-network-wired"></i>
                                <span>TCP/IP</span>
                            </div>
                            <div class="tech-icon">
                                <i class="fas fa-lock"></i>
                                <span>Security</span>
                            </div>
                            <div class="tech-icon">
                                <i class="fas fa-code-branch"></i>
                                <span>HTTP Protocol</span>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="about-image">
                    <div class="server-visualization">
                        <div class="server-icon">
                            <i class="fas fa-server"></i>
                        </div>
                        <div class="connection-lines">
                            <!-- These will be animated with JS -->
                            <div class="connection"></div>
                            <div class="connection"></div>
                            <div class="connection"></div>
                        </div>
                        <div class="client-icons">
                            <div class="client"><i class="fas fa-laptop"></i></div>
                            <div class="client"><i class="fas fa-mobile-alt"></i></div>
                            <div class="client"><i class="fas fa-desktop"></i></div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </section>

    <!-- Contact Section -->
    <section id="contact" class="contact-section">
        <div class="container">
            <div class="section-title">
                <h2>Get In Touch</h2>
                <p>Have questions about Webserv? We're here to help!</p>
            </div>
            <div class="contact-container">
                <div class="contact-form">
                    <form id="contactForm">
                        <div class="form-group">
                            <label for="name">Your Name</label>
                            <input type="text" id="name" name="name" placeholder="Enter your name" required>
                        </div>
                        <div class="form-group">
                            <label for="email">Email Address</label>
                            <input type="email" id="email" name="email" placeholder="Enter your email" required>
                        </div>
                        <div class="form-group">
                            <label for="subject">Subject</label>
                            <input type="text" id="subject" name="subject" placeholder="Enter subject" required>
                        </div>
                        <div class="form-group">
                            <label for="message">Your Message</label>
                            <textarea id="message" name="message" rows="5" placeholder="Enter your message" required></textarea>
                        </div>
                        <button type="submit" class="submit-btn">Send Message</button>
                    </form>
                </div>
                <div class="contact-info">
                    <div class="info-item">
                        <div class="info-icon">
                            <i class="fas fa-map-marker-alt"></i>
                        </div>
                        <div class="info-content">
                            <h3>Location</h3>
                            <p>123 Server Street, Web City, 94103</p>
                        </div>
                    </div>
                    <div class="info-item">
                        <div class="info-icon">
                            <i class="fas fa-envelope"></i>
                        </div>
                        <div class="info-content">
                            <h3>Email</h3>
                            <p>contact@webserv.com</p>
                        </div>
                    </div>
                    <div class="info-item">
                        <div class="info-icon">
                            <i class="fas fa-phone-alt"></i>
                        </div>
                        <div class="info-content">
                            <h3>Phone</h3>
                            <p>+123 456 7890</p>
                        </div>
                    </div>
                    <div class="info-item">
                        <div class="info-icon">
                            <i class="fas fa-clock"></i>
                        </div>
                        <div class="info-content">
                            <h3>Support Hours</h3>
                            <p>Monday - Friday: 9AM - 6PM</p>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </section>

    <footer>
        <div class="container">
            <div class="footer-content">
                <div class="footer-column">
                    <h3>Webserv</h3>
                    <p>A high-performance HTTP server written in C++ 98.</p>
                    <div class="footer-social">
                        <a href="#"><i class="fab fa-github"></i></a>
                        <a href="#"><i class="fab fa-twitter"></i></a>
                        <a href="#"><i class="fab fa-linkedin"></i></a>
                    </div>
                </div>
                <div class="footer-column">
                    <h3>Quick Links</h3>
                    <ul class="footer-links">
                        <li><a href="#home">Home</a></li>
                        <li><a href="#features">Features</a></li>
                        <li><a href="#team">Team</a></li>
                        <li><a href="#about">About</a></li>
                        <li><a href="#contact">Contact</a></li>
                    </ul>
                </div>
                <div class="footer-column">
                    <h3>Contact Us</h3>
                    <p>Email: contact@webserv.com</p>
                    <p>Phone: +123 456 7890</p>
                </div>
                <div class="footer-column">
                    <h3>Subscribe</h3>
                    <p>Stay updated with our latest news</p>
                    <form class="newsletter-form">
                        <input type="email" placeholder="Enter your email">
                        <button type="submit">Subscribe</button>
                    </form>
                </div>
            </div>
            <div class="copyright">
                <p>&copy; <span id="currentYear">2023</span> Webserv. All rights reserved.</p>
            </div>
        </div>
    </footer>

    <script>
        // Wait for DOM to load
        document.addEventListener('DOMContentLoaded', function() {
            // Loading spinner
            setTimeout(function() {
                document.getElementById('loadingSpinner').style.opacity = '0';
                setTimeout(function() {
                    document.getElementById('loadingSpinner').style.display = 'none';
                }, 500);
            }, 800);
            
            // Mobile menu toggle
            const mobileMenuBtn = document.getElementById('mobileMenuBtn');
            const navLinks = document.getElementById('navLinks');
            
            mobileMenuBtn.addEventListener('click', function() {
                navLinks.classList.toggle('active');
                
                const icon = mobileMenuBtn.querySelector('i');
                if (navLinks.classList.contains('active')) {
                    icon.classList.remove('fa-bars');
                    icon.classList.add('fa-times');
                } else {
                    icon.classList.remove('fa-times');
                    icon.classList.add('fa-bars');
                }
            });
            
            // Close mobile menu when clicking on navigation links
            const menuLinks = navLinks.querySelectorAll('a');
            menuLinks.forEach(link => {
                link.addEventListener('click', function() {
                    if (navLinks.classList.contains('active')) {
                        navLinks.classList.remove('active');
                        const icon = mobileMenuBtn.querySelector('i');
                        icon.classList.remove('fa-times');
                        icon.classList.add('fa-bars');
                    }
                });
            });
            
            // Sticky header on scroll
            const header = document.getElementById('header');
            window.addEventListener('scroll', function() {
                if (window.scrollY > 100) {
                    header.classList.add('scrolled');
                } else {
                    header.classList.remove('scrolled');
                }
            });
            
            // Current year in footer
            document.getElementById('currentYear').textContent = new Date().getFullYear();
            
            // Animated counting for stats
            const statItems = document.querySelectorAll('.stat-value');
            let animatedStats = false;
            
            function animateStats() {
                if (animatedStats) return;
                
                const statsSection = document.querySelector('.stats-section');
                const sectionPosition = statsSection.getBoundingClientRect().top;
                const screenPosition = window.innerHeight / 1.3;
                
                if (sectionPosition < screenPosition) {
                    statItems.forEach(stat => {
                        const finalValue = stat.getAttribute('data-value');
                        let startValue = 0;
                        let duration = 2000;
                        let startTime = null;
                        
                        function updateCount(timestamp) {
                            if (!startTime) startTime = timestamp;
                            const progress = timestamp - startTime;
                            const percentage = Math.min(progress / duration, 1);
                            
                            if (finalValue.includes('%')) {
                                // Percentage value
                                const numValue = parseFloat(finalValue);
                                const currentValue = (percentage * numValue).toFixed(2);
                                stat.textContent = `${currentValue}%`;
                            } else if (finalValue.includes('ms')) {
                                // Milliseconds value
                                const numValue = parseInt(finalValue);
                                const currentValue = Math.floor(percentage * numValue);
                                stat.textContent = `${currentValue}ms`;
                            } else {
                                // Regular number
                                const numValue = parseInt(finalValue);
                                const currentValue = Math.floor(percentage * numValue);
                                stat.textContent = currentValue;
                            }
                            
                            if (progress < duration) {
                                requestAnimationFrame(updateCount);
                            }
                        }
                        
                        requestAnimationFrame(updateCount);
                    });
                    
                    animatedStats = true;
                }
            }
            
            // Check if stats section is in view on scroll
            window.addEventListener('scroll', animateStats);
            // Also check on page load
            animateStats();
            
            // Form submission with validation and feedback
            const contactForm = document.getElementById('contactForm');
            if (contactForm) {
                contactForm.addEventListener('submit', function(e) {
                    e.preventDefault();
                    
                    // Form validation would go here
                    const nameField = document.getElementById('name');
                    const emailField = document.getElementById('email');
                    const subjectField = document.getElementById('subject');
                    const messageField = document.getElementById('message');
                    
                    // Simple validation example
                    if (nameField.value.trim() === '' || 
                        emailField.value.trim() === '' || 
                        subjectField.value.trim() === '' || 
                        messageField.value.trim() === '') {
                        alert('Please fill in all fields');
                        return;
                    }
                    
                    // Check for valid email
                    const emailPattern = /^[^ ]+@[^ ]+\.[a-z]{2,}$/;
                    if (!emailPattern.test(emailField.value)) {
                        alert('Please enter a valid email address');
                        return;
                    }
                    
                    // Simulate form submission
                    const submitBtn = contactForm.querySelector('.submit-btn');
                    const originalText = submitBtn.textContent;
                    submitBtn.textContent = 'Sending...';
                    submitBtn.disabled = true;
                    
                    // Simulate API call
                    setTimeout(function() {
                        submitBtn.textContent = 'Message Sent!';
                        submitBtn.style.backgroundColor = '#2ecc71';
                        
                        // Reset form
                        contactForm.reset();
                        
                        // Reset button after some time
                        setTimeout(function() {
                            submitBtn.textContent = originalText;
                            submitBtn.style.backgroundColor = '';
                            submitBtn.disabled = false;
                        }, 3000);
                    }, 1500);
                });
            }
            
            // Newsletter form submission
            const newsletterForm = document.querySelector('.newsletter-form');
            if (newsletterForm) {
                newsletterForm.addEventListener('submit', function(e) {
                    e.preventDefault();
                    
                    const emailInput = newsletterForm.querySelector('input[type="email"]');
                    const submitBtn = newsletterForm.querySelector('button');
                    
                    if (emailInput.value.trim() === '') {
                        alert('Please enter your email address');
                        return;
                    }
                    
                    const emailPattern = /^[^ ]+@[^ ]+\.[a-z]{2,}$/;
                    if (!emailPattern.test(emailInput.value)) {
                        alert('Please enter a valid email address');
                        return;
                    }
                    
                    const originalText = submitBtn.textContent;
                    submitBtn.textContent = '...';
                    submitBtn.disabled = true;
                    
                    setTimeout(function() {
                        submitBtn.textContent = 'Subscribed!';
                        emailInput.value = '';
                        
                        setTimeout(function() {
                            submitBtn.textContent = originalText;
                            submitBtn.disabled = false;
                        }, 2000);
                    }, 1000);
                });
            }
            
            // Animate server connections in About section
            function animateConnections() {
                const connections = document.querySelectorAll('.connection');
                connections.forEach((connection, index) => {
                    const delay = index * 300;
                    setInterval(() => {
                        connection.classList.add('active');
                        setTimeout(() => {
                            connection.classList.remove('active');
                        }, 1000);
                    }, 2000 + delay);
                });
            }
            
            // Only initialize if the element exists
            if (document.querySelector('.connection-lines')) {
                animateConnections();
            }
        });
        
        // Smooth scrolling for anchor links
        document.querySelectorAll('a[href^="#"]').forEach(anchor => {
            anchor.addEventListener('click', function (e) {
                e.preventDefault();
                
                const targetId = this.getAttribute('href');
                if (targetId === '#') return;
                
                const targetElement = document.querySelector(targetId);
                if (targetElement) {
                    window.scrollTo({
                        top: targetElement.offsetTop - 70,
                        behavior: 'smooth'
                    });
                }
            });
        });
    </script>
</body>
</html>
""")