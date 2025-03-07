import requests
import time

# Target URL
url = "http://127.0.0.1:3000/"

# Number of requests to send
num_requests = 10000

# Data for POST requests
post_data = {
    "username": "testuser",
    "password": "testpass"
}

# Headers (if needed)
headers = {
    "Content-Type": "application/json"
}

# Function to send GET requests
def test_get():
    for _ in range(num_requests):
        response = requests.get(url)
        print(f"GET Request - Status Code: {response.status_code}")

# Function to send POST requests
def test_post():
    for _ in range(num_requests):
        response = requests.post(url, json=post_data, headers=headers)
        print(f"POST Request - Status Code: {response.status_code}")

# Function to send DELETE requests
def test_delete():
    for _ in range(num_requests):
        response = requests.delete(url + "zaki.txt")
        print(f"DELETE Request - Status Code: {response.status_code}")

# Main function to run tests
def main():
    print("Starting GET requests test...")
    start_time = time.time()
    test_get()
    print(f"GET requests completed in {time.time() - start_time:.2f} seconds.\n")

    print("Starting POST requests test...")
    start_time = time.time()
    test_post()
    print(f"POST requests completed in {time.time() - start_time:.2f} seconds.\n")

    print("Starting DELETE requests test...")
    start_time = time.time()
    test_delete()
    print(f"DELETE requests completed in {time.time() - start_time:.2f} seconds.\n")

if __name__ == "__main__":
    main()
