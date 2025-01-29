

#include "cgi_data.hpp"


cgi_data::cgi_data()
{
}

cgi_data::~cgi_data()
{
}

void cgi_data::SetPath(std::string __path){
    _path = __path;
}


void cgi_data::SetType(std::string __type){
    _type = __type;
}


std::string cgi_data::GetPath(){
    return _path;
}


std::string cgi_data::GetType(){
    return _type;
}