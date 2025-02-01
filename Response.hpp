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
#include "Request.hpp"

#define HTTP_VERSION "HTTP/1.1"

enum Progress
{
    BUILD_RESPONSE,
    SEND_RESPONSE,
    FINISHED,
};

class Response
{
    private:
        // Response line
        std::string _httpVersion;
        int _statusCode;
        std::string _reasonPhrase;

        // headers
        std::map<std::string, std::string> _headers;

        // body
        std::string _textBody;
        std::string _file;

        std::string _response;
        size_t _totalBytesSent;
        bool _sent;

        enum Progress _progress;

    public:
        Response () : _response(""), _totalBytesSent(0), _sent(false), _progress(BUILD_RESPONSE) {};

        enum Progress getProgress() const { return _progress; };
        std::string getResponse() const { return _response; };
        size_t getTotalBytesSent() const { return _totalBytesSent; };
        void setTotalBytesSent(size_t bytes) { _totalBytesSent = bytes; };
        void setSent(bool sent) { _sent = sent; };
        void setProgress(enum Progress progress) { _progress = progress; };

        void setHttpVersion(const std::string &version);
        void setStatusCode(int status);
        void setReasonPhrase(const std::string &phrase);
        void addHeader(const std::string &key, std::string &value);
        void setTextBody(const std::string &body);
        void setFile(const std::string &filepath);
        void setContentLength(void); 

        int buildResponse(Request &request);
        bool responseSent() const { return _sent; };

        int sendResponse(int socket);
};

std::string getMimeType(const std::string& filename);
