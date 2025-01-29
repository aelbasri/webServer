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
#define CR '\r'
#define LF '\n'

enum State
{
    METHOD,
    REQUEST_TARGET,
    QUERY_KEY,
    QUERY_VALUE,
    HTTP_VERSION_,
    FORWARD_SKASH,
    DIGIT,
    DOT,
    CR_STATE,
    LF_STATE,
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
        int indexMethod;
        int indexHttp;
        char buffer[BUFF_SIZE];

        //request line
        std::string method;
        std::string requestTarget;
        std::string httpVersion;

        //query
        std::map<std::string, std::string> query;
        std::string tmpKey;
        std::string tmpValue;
        //header
        std::map<std::string, std::string> header;
    public:
        Request() : state(REQUEST_LINE), indexMethod(0),indexHttp(0) {}
        
        void handle_request(char *buffer, int bytesRec);
        
        State getState(void) const { return (state);}
        std::string getRequestTarget(void) const;
        std::string getMethod(void) const;
        std::string getHttpVersion(void) const;
        int parseRequestLine(int socket, int &offset, int &nBytes);
        int parseHeader(int socket, int &offset, int &nBytes);
        int parseBody(int socket, int &offset, int &nBytes);


        class badRequest : public std::exception 
        {

        };
};
