#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Conf.hpp"

#define CONTENT_LENGTH_LIMIT 1000000

class Connection
{
    private:
        time_t start_time;
        int _socket;
        Request _request;
        Response _response;
        server *_server;
        bool _readyToWrite;
    
    public:
        Connection() : _server(nullptr), _readyToWrite(false) {};
        Connection(int sock, server *serv);
        int sockRead();
        int getSocket() const ;
        server *getServer() ;
        bool toBeClosed()  const ;
        bool readyToWrite() const ;
        int sockWrite();
        int sendFile(bool sendInChunkFormat);
        bool sendRawBody();
        time_t getStartTime();
        void setStartTime(time_t _start_time) ;
        bool keepAlive() const ;
};
