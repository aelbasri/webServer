#include "Connection.hpp"

void Connection::sockRead()
{
    int bytesRec;
    char buffer[BUFF_SIZE];

    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;  
    if(_request.getState() == DONE)
    {
        _request.handle_request(bytesRec);
    }
    else
        _request.printRequestElement(); 
}