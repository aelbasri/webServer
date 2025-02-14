#include "Connection.hpp"
#include "Response.hpp"
#include "colors.hpp"

void Connection::sockRead()
{
    long bytesRec;
    char buffer[BUFF_SIZE];


    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;
    std::cout << RED << "bytes recived: " << bytesRec << RESET << std::endl;
    std::cout << RED << "==============" << RESET <<std::endl;
    write(1, buffer, bytesRec); 
    std::cout << RED << "==============" << RESET <<std::endl;

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
        _request.printRequestElement(); 
        // close(_socket);
        _request.closeContentFile();
    }
}

int Connection::sockWrite()
{
    if (_request.getState() != DONE || _response.getProgress() == FINISHED)
        return (0);
    if (_response.getProgress() == BUILD_RESPONSE)
    {
        try {
            _response.buildResponse(_request, _server);
        } catch (const server::InternalServerError &e) {
            setError(500, "Internal Server Error", _response, _server);
        }
        _response.createResponseStream();
    }
    if (_response.getProgress() == SEND_RESPONSE)
    {
        ssize_t bytesSent = send(_socket, _response.getResponse().c_str() + _response.getTotalBytesSent(), _response.getResponse().size() - _response.getTotalBytesSent(), 0);
        if (bytesSent == -1)
            return (-1);
        _response.setTotalBytesSent(_response.getTotalBytesSent() + bytesSent);
        if (_response.getTotalBytesSent() == _response.getResponse().size())
        {
            _response.setSent(true);
            _response.setProgress(FINISHED);
            std::cout << "Response Done" << std::endl;
        }
    }
    return (0);
}
