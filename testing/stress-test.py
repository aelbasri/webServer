#!/usr/bin/env python3
import requests
import time
import random
import argparse
import concurrent.futures
import sys
from collections import Counter
from tqdm import tqdm

# Colors for terminal output
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    
    @staticmethod
    def status_color(status_code):
        """Return color based on HTTP status code"""
        if status_code == 0:
            return Colors.RED  # Error
        elif 200 <= status_code < 300:
            return Colors.GREEN  # Success
        elif 300 <= status_code < 400:
            return Colors.BLUE  # Redirection
        elif 400 <= status_code < 500:
            return Colors.YELLOW  # Client Error
        elif 500 <= status_code < 600:
            return Colors.RED  # Server Error
        return Colors.ENDC
    
    @staticmethod
    def percentage_color(percentage):
        """Return color based on percentage value"""
        if percentage >= 90:
            return Colors.GREEN
        elif percentage >= 70:
            return Colors.BLUE
        elif percentage >= 50:
            return Colors.CYAN
        elif percentage >= 30:
            return Colors.YELLOW
        else:
            return Colors.RED

def send_request(request_type, url, timeout=5, data=None):
    """Send a single HTTP request and return the result."""
    start_time = time.time()
    try:
        if request_type == "GET":
            response = requests.get(url, timeout=timeout)
        elif request_type == "POST":
            response = requests.post(url, data=data or {"data": "stress test payload"}, timeout=timeout)
        elif request_type == "DELETE":
            response = requests.delete(url, timeout=timeout)
        else:
            raise ValueError(f"Unsupported request type: {request_type}")
        
        elapsed = time.time() - start_time
        return {
            "status_code": response.status_code,
            "success": 200 <= response.status_code < 400,
            "time": elapsed
        }
    except Exception as e:
        elapsed = time.time() - start_time
        return {
            "status_code": 0,
            "success": False,
            "time": elapsed,
            "error": str(e)
        }

def run_stress_test(base_url, num_requests=100, concurrency=10, request_types=None, timeout=5):
    """Run a stress test with the specified parameters."""
    if request_types is None:
        request_types = ["GET", "POST", "DELETE"]
    
    results = []
    total_start_time = time.time()
    
    print(f"{Colors.HEADER}{Colors.BOLD}Starting stress test on {Colors.UNDERLINE}{base_url}{Colors.ENDC}")
    print(f"Sending {Colors.BOLD}{num_requests}{Colors.ENDC} requests with concurrency={Colors.BOLD}{concurrency}{Colors.ENDC}")
    print(f"Request types: {Colors.CYAN}{', '.join(request_types)}{Colors.ENDC}")
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=concurrency) as executor:
        futures = []
        
        # Create all the request futures
        for i in range(num_requests):
            request_type = random.choice(request_types)
            
            # Create some random data for POST
            data = None
            if request_type == "POST":
                data = {
                    "id": i,
                    "timestamp": time.time(),
                    "payload": f"Stress test data payload {i}" * random.randint(1, 10)
                }
            
            futures.append(
                executor.submit(send_request, request_type, base_url, timeout, data)
            )
        
        # Process results as they complete
        for future in tqdm(concurrent.futures.as_completed(futures), total=len(futures), desc="Requests", 
                          bar_format="{l_bar}%s{bar}%s{r_bar}" % (Colors.BLUE, Colors.ENDC)):
            try:
                result = future.result()
                results.append(result)
            except Exception as e:
                print(f"{Colors.RED}Error processing result: {e}{Colors.ENDC}")
    
    total_time = time.time() - total_start_time
    return results, total_time

def analyze_results(results, total_time):
    """Analyze and print test results."""
    status_counts = Counter([r["status_code"] for r in results])
    success_count = sum(1 for r in results if r["success"])
    failure_count = len(results) - success_count
    
    # Calculate timing statistics
    if results:
        times = [r["time"] for r in results]
        avg_time = sum(times) / len(times)
        max_time = max(times)
        min_time = min(times)
    else:
        avg_time = max_time = min_time = 0
    
    success_percent = success_count/len(results)*100 if results else 0
    failure_percent = failure_count/len(results)*100 if results else 0
    
    print(f"\n{Colors.HEADER}{Colors.BOLD}--- STRESS TEST RESULTS ---{Colors.ENDC}")
    print(f"Total requests: {Colors.BOLD}{len(results)}{Colors.ENDC}")
    print(f"Successful responses: {Colors.percentage_color(success_percent)}{success_count} ({success_percent:.2f}%){Colors.ENDC}")
    print(f"Failed responses: {Colors.percentage_color(100-failure_percent)}{failure_count} ({failure_percent:.2f}%){Colors.ENDC}")
    print(f"Total test time: {Colors.BOLD}{total_time:.2f} seconds{Colors.ENDC}")
    print(f"Requests per second: {Colors.BOLD}{len(results)/total_time:.2f}{Colors.ENDC}")
    
    print(f"\n{Colors.HEADER}{Colors.BOLD}--- TIMING STATISTICS ---{Colors.ENDC}")
    print(f"Average response time: {Colors.CYAN}{avg_time:.4f} seconds{Colors.ENDC}")
    print(f"Minimum response time: {Colors.GREEN}{min_time:.4f} seconds{Colors.ENDC}")
    print(f"Maximum response time: {Colors.YELLOW}{max_time:.4f} seconds{Colors.ENDC}")
    
    print(f"\n{Colors.HEADER}{Colors.BOLD}--- STATUS CODE DISTRIBUTION ---{Colors.ENDC}")
    for status_code in sorted(status_counts.keys()):
        count = status_counts[status_code]
        percentage = count / len(results) * 100
        status_color = Colors.status_color(status_code)
        if status_code == 0:
            print(f"{status_color}Request errors: {count} ({percentage:.2f}%){Colors.ENDC}")
        else:
            print(f"{status_color}Status {status_code}: {count} ({percentage:.2f}%){Colors.ENDC}")
    
    # Print some error samples if there were any
    errors = [r for r in results if "error" in r]
    if errors:
        print(f"\n{Colors.HEADER}{Colors.BOLD}--- SAMPLE ERRORS ---{Colors.ENDC}")
        for i, error in enumerate(errors[:5]):  # Show max 5 errors
            print(f"{Colors.RED}Error {i+1}: {error['error']}{Colors.ENDC}")
        if len(errors) > 5:
            print(f"{Colors.YELLOW}... and {len(errors) - 5} more errors{Colors.ENDC}")

def main():
    parser = argparse.ArgumentParser(description="Web Server Stress Testing Tool")
    parser.add_argument("--url", default="http://localhost:3000", help="Target URL (default: http://localhost:3000)")
    parser.add_argument("--requests", type=int, default=100, help="Number of requests to send (default: 100)")
    parser.add_argument("--concurrency", type=int, default=10, help="Number of concurrent requests (default: 10)")
    parser.add_argument("--timeout", type=float, default=5.0, help="Request timeout in seconds (default: 5.0)")
    parser.add_argument("--types", default="GET,POST,DELETE", help="Comma-separated request types (default: GET,POST,DELETE)")
    parser.add_argument("--no-color", action="store_true", help="Disable colored output")
    
    args = parser.parse_args()
    
    # Disable colors if requested
    if args.no_color:
        for attr in dir(Colors):
            if not attr.startswith('__'):
                setattr(Colors, attr, '')
    
    # Check if terminal supports colors
    if not sys.stdout.isatty():
        for attr in dir(Colors):
            if not attr.startswith('__'):
                setattr(Colors, attr, '')
    
    # Parse request types
    request_types = [t.strip().upper() for t in args.types.split(",")]
    for request_type in request_types:
        if request_type not in ["GET", "POST", "DELETE"]:
            print(f"{Colors.RED}Unsupported request type: {request_type}{Colors.ENDC}")
            print(f"Supported types: {Colors.GREEN}GET{Colors.ENDC}, {Colors.BLUE}POST{Colors.ENDC}, {Colors.YELLOW}DELETE{Colors.ENDC}")
            sys.exit(1)
    
    try:
        results, total_time = run_stress_test(
            args.url, 
            num_requests=args.requests,
            concurrency=args.concurrency,
            request_types=request_types,
            timeout=args.timeout
        )
        analyze_results(results, total_time)
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}Test interrupted by user.{Colors.ENDC}")
        sys.exit(0)
    except Exception as e:
        print(f"\n{Colors.RED}Error during stress test: {e}{Colors.ENDC}")
        sys.exit(1)

if __name__ == "__main__":
    main()
