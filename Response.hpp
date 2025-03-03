#pragma once

#include "configfile/server_data.hpp"
#include "Request.hpp"
#include <numeric>
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
#include <sys/stat.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <dirent.h>

#define HTTP_VERSION "HTTP/1.1"
#define RESPONSE_CHUNCK_SIZE 1024
#define BUFFER_SIZE 100

enum Progress
{
    BUILD_RESPONSE,
    POST_HOLD,
    SEND_RESPONSE,
    SEND_HEADERS,
    SEND_BODY,
    FINISHED,
};

enum FileState {
    FILE_DOES_NOT_EXIST,
    FILE_IS_REGULAR,
    FILE_IS_DIRECTORY,
    FILE_IS_OTHER
};

struct responseBodyFile
{
    size_t offset;
    size_t nBytes;
    size_t consumed;
    char buffer[BUFFER_SIZE];
    std::ifstream file;
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
        responseBodyFile *_fileBody;

    public:
        Response () : _response(""), _totalBytesSent(0), _sent(false), _progress(BUILD_RESPONSE), _fileBody(nullptr) {};
        ~Response() { if (_fileBody) { delete _fileBody; _fileBody = nullptr;} };
        enum Progress getProgress() const { return _progress; };
        std::string getResponse() const { return _response; };
        responseBodyFile *getFileBody() { return _fileBody; };
        std::string getTextBody() const { return _textBody; };
        size_t getTotalBytesSent() const { return _totalBytesSent; };
        int getStatusCode() const { return _statusCode; };
        int setFileBody(std::string path);
        void setTotalBytesSent(size_t bytes) { _totalBytesSent = bytes; };
        void setSent(bool sent) { _sent = sent; };
        void setProgress(enum Progress progress) { _progress = progress; };

        void setHttpVersion(const std::string &version);
        void setStatusCode(int status);
        void setReasonPhrase(const std::string &phrase);
        void addHeader(const std::string &key, const std::string &value);
        void setTextBody(const std::string &body);
        void setFile(const std::string &filepath);
        void setContentLength(int length); 

        void buildResponse(Request &request, server *serv);
        void createResponseStream(std::string &connectionHeader);
        bool responseSent() const { return _sent; };

        int sendResponse(int socket);

        void processPOST(Request &request, location *locationMatch);
        void processGET(Request &request, std::string &path);
        void processDELETE(Request &request, server *serv, std::string &path);

        void processDirectoryRequest(Request &request, location *locationMatch, server *serv, std::string &path);
};

std::string getMimeType(const std::string& filename);
location* getLocationMatch(std::string target, location *locations, int size);
bool methodAllowed(const std::string& method, const std::vector<std::string>& allowedMethods);
void setHttpResponse(int status, std::string message, Response &response, server *serv);
int parseCGI(std::string &CgiOutput, Response &response);
FileState getFileState(const char *path);
std::string listDirectoryHTML(const char *path);
std::string getFilenameFromPath(std::string path);
void handleCGI(Response &response, Request &request);


