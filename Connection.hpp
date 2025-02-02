#pragma once

#include "Request.hpp"
#include "Response.hpp"

class Connection
{
    private:
        int _socket;
        Request _request;
        Response _response;
        // bool _isComplete;
    
    public:
        Connection() {};
        Connection(int sock) : _socket(sock) {};
        void sockRead();
        int getSocket() const {return _socket;};
        bool toBeClosed()  const {return (_request.getState() == DONE && _response.getProgress() == FINISHED);};
        void sockWrite();
};
