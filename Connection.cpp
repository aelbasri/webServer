#include "Connection.hpp"

void Connection::sockRead()
{
    long bytesRec;
    char buffer[BUFF_SIZE];

    std::cout << "=======bytes recived" << std::endl;
    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;
    // write(1, buffer, bytesRec); 
    std::cout << "=======end:" << std::endl;
    std::cout << "nbytes: " << bytesRec << std::endl; 
    _request.handle_request(buffer, bytesRec);
    //after completing the request
    if(_request.getState() == DONE)
    {
        _request.printRequestElement(); 
        close(_socket);
        _request.closeContentFile();
    }
}