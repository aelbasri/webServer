#pragma once

#include <string>
#include <cstring>
#include <cstdio>
#include <map>
#include <cmath>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "Request.hpp"
#include "configfile/server_data.hpp"

#define HTTP_VERSION "HTTP/1.1"
#define RESPONSE_CHUNCK_SIZE 1024

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
        int buildResponse2(Request &request, server &serv);
        int createResponseStream();
        bool responseSent() const { return _sent; };

        int sendResponse(int socket);
};

std::string getMimeType(const std::string& filename);
location* getLocationMatch(std::string target, location *locations, int size);
bool methodAllowed(const std::string& method, const std::vector<std::string>& allowedMethods);
int setError(int status, std::string message, Response &response, server &serv);
