#pragma once

#include "server_data.hpp"

class cgi_data
{
private:
   std::string _type;
   std::string _path;

public:
    cgi_data();
    ~cgi_data();

    void SetPath(std::string __type);
    void SetType(std::string __indix);

    std::string GetPath();
    std::string GetType();
};

