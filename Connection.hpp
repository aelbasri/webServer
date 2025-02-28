#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Conf.hpp"

class Connection
{
    private:
        int _socket;
        Request _request;
        Response _response;
        server *_server;
        bool _readyToWrite;
    
    public:
        Connection() : _server(nullptr), _readyToWrite(false) {};
        Connection(int sock, server *serv) : _socket(sock), _server(serv), _readyToWrite(false) {};
        void sockRead();
        int getSocket() const {return _socket;};
        bool toBeClosed()  const {return (_request.getState() == DONE && _response.getProgress() == FINISHED);};
        bool readyToWrite() const {return _readyToWrite;};
        int sockWrite();
        int sendFile(bool sendInChunkFormat);
        bool sendRawBody();
};

// int sendFile(Connection cnx, Response response, bool sendInChunkFormat);
