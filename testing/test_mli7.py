#!/usr/bin/env python3
import sys
import requests
import psutil
from termcolor import colored
from concurrent.futures import ThreadPoolExecutor
import os
import time

# Install missing packages
try:
    from termcolor import colored
except ImportError:
    os.system("pip install termcolor")
    from termcolor import colored

try:
    import psutil
except ImportError:
    os.system("pip install psutil")
    import psutil

# Configuration
TARGET_URL = "http://127.0.0.1:3000/saad.txt"
NUM_REQUESTS = 1000
CONCURRENT_WORKERS = 100
POST_DATA = {"username": "testuser", "password": "testpass"}
HEADERS = {"Content-Type": "application/json"}

class ResourceMonitor:
    def __init__(self):
        self.process = psutil.Process(os.getpid())
        self.initial_fd = self.process.num_fds()
        self.initial_connections = len(self.process.connections())

    def check_leaks(self):
        current_fd = self.process.num_fds()
        current_conn = len(self.process.connections())
        
        fd_leak = current_fd - self.initial_fd
        conn_leak = current_conn - self.initial_connections
        
        return fd_leak, conn_leak

def color_status(status_code):
    if 200 <= status_code < 300:
        return colored(str(status_code), "green")
    elif 300 <= status_code < 400:
        return colored(str(status_code), "yellow")
    elif 400 <= status_code < 500:
        return colored(str(status_code), "red")
    else:
        return colored(str(status_code), "magenta")

def make_request(method):
    session = requests.Session()
    try:
        if method == "GET":
            response = session.get(TARGET_URL)
        elif method == "POST":
            response = session.post(TARGET_URL, json=POST_DATA, headers=HEADERS)
        elif method == "DELETE":
            response = session.delete(TARGET_URL)
        
        status_code = response.status_code
        colored_code = color_status(status_code)
        print(f"{colored(method, 'blue')} {colored_code}")
        
    except Exception as e:
        print(colored(f"{method} ERROR: {str(e)}", "magenta"))
    finally:
        session.close()

def stress_test(method):
    with ThreadPoolExecutor(max_workers=CONCURRENT_WORKERS) as executor:
        futures = [executor.submit(make_request, method) for _ in range(NUM_REQUESTS)]
        for future in futures:
            future.result()  # Wait for all requests to complete

def main():
    methods = ["GET", "POST", "DELETE"]
    print(colored("\n=== Starting Load Test ===", "cyan"))
    print(colored(f"Target URL: {TARGET_URL}", "cyan"))
    print(colored(f"Total requests per method: {NUM_REQUESTS}", "cyan"))
    print(colored(f"Concurrent workers: {CONCURRENT_WORKERS}\n", "cyan"))
    
    # Initial system state
    initial_monitor = ResourceMonitor()
    
    for method in methods:
        start_time = time.time()
        print(colored(f"\n=== Testing {method} ===", "blue"))
        stress_test(method)
        duration = time.time() - start_time
        
        print(colored(f"\n{method} Summary:", "yellow"))
        print(f"Total time: {duration:.2f}s")
        print(f"Requests/sec: {NUM_REQUESTS/duration:.2f}")
    
    # Final system state
    final_monitor = ResourceMonitor()
    fd_leak = final_monitor.process.num_fds() - initial_monitor.initial_fd
    conn_leak = len(final_monitor.process.connections()) - initial_monitor.initial_connections
    
    print(colored("\n=== Final System Check ===", "cyan"))
    print(colored(f"Total FD leaks: {fd_leak}", "red" if fd_leak else "green"))
    print(colored(f"Total connection leaks: {conn_leak}", "red" if conn_leak else "green"))

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(colored("\nTest aborted by user!", "magenta"))
        sys.exit(1)
