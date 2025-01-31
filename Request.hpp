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
    REQUEST_LINE,
    REQUEST_TARGET,
    QUERY_KEY,
    QUERY_VALUE,
    HTTP_VERSION_,
    FORWARD_SKASH,
    DIGIT,
    DOT,
    CR_STATE,
    LF_STATE,
    HEADER,
    FIELD_NAME,
    OWS,
    FIELD_VALUE,
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
        State mainState;
        State subState;


        int indexMethod;
        int indexHttp;
        // char buffer[BUFF_SIZE];

        //request line
        std::string method;
        std::string requestTarget;
        std::string httpVersion;

        //query
        std::map<std::string, std::string> query;
        std::string queryName;
        std::string queryValue;

        //header
        std::map<std::string, std::string> headers;
        std::string fieldName;
        std::string fieldValue;

        //body
        int consumed;

    public:
        Request() : mainState(REQUEST_LINE), subState(METHOD), indexMethod(0),indexHttp(0), fieldName(""), fieldValue("") {}
        
        void handle_request(char *buffer, int bytesRec);
        
        State getState(void) const { return (mainState);}
        std::string getRequestTarget(void) const;
        std::string getMethod(void) const;
        std::string getHttpVersion(void) const;
        // int parseRequestLine(int socket, int &offset, int &nBytes);
        // int parseHeader(int socket, int &offset, int &nBytes);
        // int parseBody(int socket, int &offset, int &nBytes);
        void parseRequestLine(char *buffer, int i);
        void parseHeader(char *buffer, int i);
        void parseBody(char *buffer, int i, int bytesRec);

        //delete me
        void printRequestElement();


        class badRequest : public std::exception 
        {

        };
};
