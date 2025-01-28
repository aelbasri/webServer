#include "Connection.hpp"

void Connection::sockRead()
{
    int bytesRec;
    char buffer[BUFF_SIZE];

    if((bytesRec = recv(_socket, buffer, BUFF_SIZE, 0)) <= 0)
        return ;  
    if(!_isComplete)
    {
        _request.handle_request(buffer, bytesRec);
    }
}