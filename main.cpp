#include "server.hpp"
#include <iostream>

int main()
{
    Server _servrer;

    try
    {
        Server server;
        std::cout << getpid() << std::endl;
        server.run();
        exit(0);
        // server.printRequest();

     }
     catch (runtime_error e)
     {
         cout << "L***A JAAAY!: " << e.what();
     }
    return (0);
}
