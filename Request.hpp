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
    CONTLEN,
    CHUNKS,
    CHUNK_HEADER,
    LOAD_CHUNK,
    WAIT,
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

        char buffer[BUFF_SIZE];
        long bytesRec;
        long offset;

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
        long consumed;
        std::ofstream contentFile;
        std::string _contentFile;
        long long contentBodySize;
        long contentLength;

        std::string chunkSizeS;
        long chunkSizeL;
        long long maxBodySize;



    public:
        //TODO: 9aad l constructure adak ras lbo9ala
        Request() : mainState(REQUEST_LINE), subState(METHOD), bytesRec(0), offset(0), indexMethod(0),indexHttp(0), fieldName(""), fieldValue(""), consumed(0), _contentFile(""), contentBodySize(0) { memset(buffer, 0, BUFF_SIZE); }
        
        void handle_request(char *buffer);
        
        State getState(void) const { return (mainState);}
        std::string getMethod(void) { return (method); }
        std::string getContentFile(void) { return (_contentFile); }
        long getOffset(void) const { return (offset); }
        long getBytesRec(void) const { return (bytesRec); }
        std::string getRequestTarget(void) const;
        std::string getMethod(void) const;
        std::string getHttpVersion(void) const;
        char *getBuffer() { return buffer; }

        void setContentFile(std::string contentFile) { _contentFile = contentFile; }
        void setMaxBodySize(long long size) { maxBodySize = size; }
        void setState(State state) { mainState = state; }
        void setBytrec(long _bytesRec) { bytesRec = _bytesRec; }
        void setOffset(long _offset) { offset = _offset; }
        void setBuffer() {memset(buffer, 0, BUFF_SIZE);}

        bool HostHeaderExists() const
        {
            std::map<std::string, std::string>::const_iterator it = headers.find("Host");
            return (it != headers.end());
        }

        std::string getHostHeader() const
        {
            std::map<std::string, std::string>::const_iterator it = headers.find("Host");
            if (it != headers.end())
                return it->second;
            return ""; // should never happen
        }

        void parseRequestLine(char *buffer, long i);
        void parseHeader(char *buffer, long i);
        void parseBody(char *buffer, long &i, long bytesRec);

        void closeContentFile();

        //delete me
        void printRequestElement();


        class badRequest : public std::exception 
        {
            public :
                const char *what() const throw()
                {
                    return ("Bad request");
                }
        };
};
