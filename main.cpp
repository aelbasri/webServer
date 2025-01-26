#include "server.hpp"
#include <iostream>

int main()
{
    try
    {
        servers _Server("config_file.yaml");
        _Server.loadContentServer();
        _Server.run();
        // exit(0);
        _Server.printRequest();

     }
     catch (std::runtime_error e)
     {
         std::cout << "L***A JAAAY!: " << e.what();
     }
    return (0);
}
