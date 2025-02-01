#include "Connection.hpp"
#include "Response.hpp"

void Connection::sockRead()
{
    int bytesRec;
    char buffer[BUFF_SIZE];

    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;
    std::cout << "=======bytes recived" << std::endl;
    write(1, buffer, bytesRec); 
    std::cout << "=======end:" << std::endl;
    std::cout << "nbytes: " << bytesRec << std::endl; 
    _request.handle_request(buffer, bytesRec);
    std::cout << "Request handled" << std::endl;
    // exit(11);
    if(_request.getState() == DONE)
    {
        _request.printRequestElement(); 
        std::cout << "Request printed" << std::endl;
        // close(_socket);
    }
}

void Connection::sockWrite()
{
    if (_request.getState() != DONE || _response.getProgress() == FINISHED)
        return;
    if (_response.getProgress() == BUILD_RESPONSE)
    {
        _response.buildResponse(_request);
    }
    if (_response.getProgress() == SEND_RESPONSE)
    {
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
            // _response.setTotalBytesSent(0);
            // _response.setResponse("");
            // _response.setHttpVersion("");
            // _response.setStatusCode(0);
            // _response.setReasonPhrase("");
            // _response.getHeaders().clear();
            // _response.setTextBody("");
            // _response.setFile("");
            _response.setProgress(FINISHED);
        }
    }
}
