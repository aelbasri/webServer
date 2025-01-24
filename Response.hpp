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

#define HTTP_VERSION "HTTP/1.1"

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

    public:
        void setHttpVersion(const std::string &version);
        void setStatusCode(int status);
        void setReasonPhrase(const std::string &phrase);
        void addHeader(const std::string &key, std::string &value);
        void setTextBody(const std::string &body);
        void setFile(const std::string &filepath);
        void setContentLength(void); 
        void sendResponse(int socket) const;
};
