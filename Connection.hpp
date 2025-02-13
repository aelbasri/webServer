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
        // bool _isComplete;
    
    public:
        Connection() : _server(nullptr) {};
        Connection(int sock, server *serv) : _socket(sock), _server(serv) {};
        void sockRead();
        int getSocket() const {return _socket;};
        bool toBeClosed()  const {return (_request.getState() == DONE && _response.getProgress() == FINISHED);};
        void sockWrite();
};
