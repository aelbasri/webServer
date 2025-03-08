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
        Connection(int sock, server *serv) : _socket(sock), _server(serv), _readyToWrite(false) {
            // exit(101);
            _request.setMaxBodySize(_server->Get_max_body_size());
            _response.setSocket(sock);
        };
        int sockRead();
        int getSocket() const {return _socket;};
        server *getServer() { return _server; };
        bool toBeClosed()  const {
            if (_request.getMethod() == "DELETE")
                return (_response.getProgress() == FINISHED);
            else
                return (_request.getState() == DONE && _response.getProgress() == FINISHED);
        };
        bool readyToWrite() const {return _readyToWrite;};
        int sockWrite();
        int sendFile(bool sendInChunkFormat);
        bool sendRawBody();

        time_t getStartTime() { return start_time; };
        void setStartTime(time_t _start_time) {
            start_time = _start_time;
        }

        bool keepAlive() const {
            std::string conn = "Connection";
             return _request.getHeader(conn) != "close";
        }
};


void deleteTimedoutSockets(std::map<int, Connection* > &connections, int ep);