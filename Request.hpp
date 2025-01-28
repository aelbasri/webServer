#pragma once

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <map>
#include <cmath>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define BUFF_SIZE 1024

enum State
{
    METHOD,
    REQUEST_TARGET,
    HTTP_VERSION,
    REQUEST_LINE,
    HEADER,
    BODY,
    DONE
};

struct parseBodyElement
{
    int offset;
    int nBytes;
    int consumed;
    char *buffer;
    std::ofstream file;
};

class Request
{
    private:
        State state;
        int index;
        char buffer[BUFF_SIZE];
        std::string tmp;

        //request line
        std::string method;
        std::string requestTarget;
        std::string httpVersion;

        //header
        std::map<std::string, std::string> header;
    public:
        Request() : state(REQUEST_LINE), index(0) {}
        
        void handle_request(char *buffer, int bytesRec);
        
        State getState(void) const { return (state);}
        int getIndex(void) const { return (index);}
        std::string getRequestTarget(void) const;
        std::string getMethod(void) const;
        std::string getHttpVersion(void) const;
        int parseRequestLine(int socket, int &offset, int &nBytes);
        int parseHeader(int socket, int &offset, int &nBytes);
        int parseBody(int socket, int &offset, int &nBytes);


        class badRequest : public std::exception 
        {
            public:
                
        };
};
