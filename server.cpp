#include "server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <stdlib.h>

#define FILE_PATH "./assets/page.html"


Server::Server() : _host("127.0.0.1"), _port("3000") {}

Server::Server(std::string host, std::string port) : _host(host), _port(port) {}

const char* Server::InternalServerError::what() const throw()
{
    return("Saad Ka3i, 3lach??");
}

int Server::run()
{
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int yes = 1;
    int addI;

    memset(&hints, 0,  sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((addI = getaddrinfo(NULL, _port.c_str(), &hints, &res)) != 0)
    {
        std::cerr << gai_strerror(addI) << std::endl;
        return -1;
    }

    for(p = res; p; p = p->ai_next)
    {
        if ((_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
            continue;
        if ((setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1)
        {
            perror("setsockopt");
            return (-1);
            /*exit(1);*/
        }
        //bind
        if (bind(_sock, res->ai_addr, res->ai_addrlen) == -1)
            continue;
        break;
    }

    freeaddrinfo(res); 
    if (!p)
    {
        perror("bind failed");
        return (-1);
        /*exit(1);*/
    }

    if (listen(_sock, 10) == -1)
    {
        perror("listen() failed");
        return (-1);
        /*exit(1);*/
    }

    std::cout << "Server is listening on " << _host << ":" << _port << std::endl; 
    return (0);
}

void send_res(Request &request, int new_fd)
{
    Response response;
    std::string path("./assets");
    
    if (request.getRequestTarget() == "/")
        path += "/index.html";
    else
        path += request.getRequestTarget();
    std::ifstream file(path.c_str());

                    std::cout << "PATH: "  << path << std::endl;


    //int res = access(path.c_str(), F_OK | R_OK);
    if (!file.good()) {
        std::cout << "iror nat found" << std::endl;
        path = "./assets/404.html";
        std::string connection = "close";
        std::string contentType = getMimeType(path);

        response.setHttpVersion(HTTP_VERSION);
        response.setStatusCode(404);
        response.setReasonPhrase("Not Found");
        response.setFile(path);

        response.setContentLength();
        response.addHeader(std::string("Content-Type"), contentType);
        response.addHeader(std::string("Connection"), connection);
        response.sendResponse(new_fd);
    }
    else
    {
        std::cout << "saad t3asb "  << path << std::endl;

        std::string connection = "close";
        std::string contentType = getMimeType(path);

        response.setHttpVersion(HTTP_VERSION);
        response.setStatusCode(200);
        response.setReasonPhrase("OK");
        response.setFile(path);

        response.setContentLength();
        response.addHeader(std::string("Content-Type"), contentType);
        response.addHeader(std::string("Connection"), connection);
        response.sendResponse(new_fd);
    }
}

void Server::printRequest() const
{
    while(1)
    {
        int new_fd = accept(_sock, NULL, 0);
        std::cout << "===> connection accepted" << std::endl;
        
        if (new_fd == -1)
        {
            perror("accept() failed");
            exit(1);
        }
        
        /*Parse Request*/

        try
        {
            Request request;
            int offset = 0;
            int nBytes = 0;
            enum state myState = REQUEST_LINE;


            while(myState != DONE)
            {
                switch (myState)
                {
                    case  REQUEST_LINE :
                        request.parseRequestLine(new_fd, offset, nBytes);
                        myState = HEADER;
                        break;
                    case HEADER :
                        request.parseHeader(new_fd, offset, nBytes);
                        myState = BODY;
                        break;
                    case BODY :
                        request.parseBody(new_fd, offset, nBytes);
                        myState = DONE;
                        break;
                    default:
                        break;
                }
            }
                        throw Server::InternalServerError(); 

            send_res(request, new_fd);
        }
        catch(const std::exception& e)
        {
            Response response;
            std::cout << "Saad 500Error" << std::endl;
            std::string path = "./assets/50x.html";
            std::string connection = "close";
            std::string contentType = getMimeType(path);

            response.setHttpVersion(HTTP_VERSION);
            response.setStatusCode(500);
            response.setReasonPhrase("Internal Server Error");
            response.setFile(path);

            response.setContentLength();
            response.addHeader(std::string("Content-Type"), contentType);
            response.addHeader(std::string("Connection"), connection);
            response.sendResponse(new_fd);
            // std::cerr << e.what() << std::endl;
        }
    
        

        /* Use the Response class */
        // Response response;
        // std::string version = "HTTP/1.1";
        // int status = 200;
        // std::string reasonPhrase = "OK";
        // std::string contentType = "text/html; charset=UTF-8";

        // std::stringstream ss;
        // {
        //     // _textBody size
        //     /*response.setTextBody(html_content);*/
        //     /*ss << _textBody.size();*/
        // }
        // {
        //     // _file size
        //     response.setFile(FILE_PATH);
        //     /*ss << _file.size();*/
        // }
        // std::string contentLength = ss.str();
        // std::string connection = "close";

        // response.setHttpVersion(version);
        // response.setStatusCode(status);
        // response.setReasonPhrase(reasonPhrase);
        // response.addHeader(std::string("Content-Type"), contentType);
        // /*response.addHeader(std::string("Content-Length"), contentLength);*/
        // response.setContentLength();
        // response.addHeader(std::string("Connection"), connection);
        // response.sendResponse(new_fd);


        close(new_fd);
    }
}
