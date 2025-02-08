#include "Connection.hpp"
#include <cstddef>

void Connection::sockRead()
{
    long bytesRec;
    char buffer[BUFF_SIZE];

    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;
    std::cout << "=======bytes recived: " << bytesRec << std::endl;
    write(1, buffer, bytesRec); 
    std::cout << "===============================================" << std::endl;

    try
    {
        _request.handle_request(buffer, bytesRec);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    
    //after completing the request
    if(_request.getState() == DONE)
    {
        std::cout << "Request Done" << std::endl;
        // _request.printRequestElement(); 
        // close(_socket);
        _request.closeContentFile();
    }
}

void Connection::sockWrite()
{
    if (!_server)
        throw std::runtime_error("Server not found");
    if (_request.getState() != DONE || _response.getProgress() == FINISHED)
        return;
    if (_response.getProgress() == BUILD_RESPONSE)
    {
        _response.buildResponse2(_request, _server);
        _response.createResponseStream();
        // _response.buildResponse(_request);
    }
    if (_response.getProgress() == SEND_RESPONSE)
    {
        // _response.sendResponse(_socket);
        ssize_t bytesSent = send(_socket, _response.getResponse().c_str() + _response.getTotalBytesSent(), _response.getResponse().size() - _response.getTotalBytesSent(), 0);
        if (bytesSent == -1)
        {
            // Throw exception
            return;
        }
        _response.setTotalBytesSent(_response.getTotalBytesSent() + bytesSent);
        if (_response.getTotalBytesSent() == _response.getResponse().size())
        {
            _response.setSent(true);
            _response.setProgress(FINISHED);
            std::cout << "Response Done" << std::endl;
        }
    }
}
