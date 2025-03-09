


# webServ

## Overview

**webserv** is a minimal implementation of the HTTP/1.1 protocol, developed as part of the 42 project curriculum. This project is written in C++98 and aims to provide a lightweight, high-performance web server that adheres to the specifications of HTTP/1.1.

## Features

- **HTTP/1.1 Compliance**: Fully supports the HTTP/1.1 protocol, including persistent connections, chunked transfer encoding, and more.
- **Static File Serving**: Capable of serving static files from a specified directory.
- **Configurable**: Easily configurable through a settings file to define server parameters and routes.
- **Error Handling**: Comprehensive error handling for common HTTP errors (404, 500, etc.).


## Getting Started

### Prerequisites

- **C++98 Compiler**: Ensure you have a compatible C++98 compiler installed.
- **Make**: Required for building the project.

### Installation

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/aelbasri/webServer.git
    cd webServer
    ```

2. **Build the Project**:
    ```bash
    make
    ```

### Running the Server

Once the project is built, you can run the server with the following command:

```bash
./webserv [CONFIG_FILE_PATH]
```

### Configuration

The server can be configured using a configuration file. Here is an example configuration in yaml format:

```conf
server:
    name :  "webServ"     
    host: 127.0.0.1
    port:   3000
    port:   4000
    port:   5000
    port:   6000


    max_body_size :       1456

    error_pages:
      404: ./assets/errors/404.html
      500: ./assets/errors/50x.html
      302:  ./assets/errors/302.html
    location :
      type : /
      directoryListing : on
      root_directory: ./assets/web-folder/web
      allowed_methods:
        - POST
        - DELETE
        - GET
      index  : index.html
      index  : index2.html

    CGI:
      path :  ./cgi-bin/
```

## Usage

After starting the server, you can access it via a web browser or using tools like `curl`:

```bash
curl http://localhost
```
