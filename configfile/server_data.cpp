/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:58:24 by zel-khad          #+#    #+#             */
/*   Updated: 2025/02/25 11:02:49 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_data.hpp"

server& server::operator=(const server &server)
{
    if (this == &server)
        return *this;

    _number_of_location = server._number_of_location;
    // location is pointer, so we need to copy the content of the pointer
    if (_location)
        delete[] _location;
    _location = new location[_number_of_location];
    for (int i = 0; i < _number_of_location; i++)
        _location[i] = server._location[i];

    _content = server._content;
    _name = server._name;
    _host = server._host;
    // _port = server._port;
    _max_body_size = server._max_body_size;
    _sock = server._sock;
    hints = server.hints;
    res = server.res;
    p = server.p;
    addI = server.addI;
    return *this;
}

server::server(){
    _name = "localhost";
    _sock.push_back(std::make_pair("8080", 0));
    _NPort = 0;
    _location = nullptr;
    _indixL = 0;
    _max_body_size = 1048576;
    _host = "127.0.0.0";
}

server::~server() {
    if (_location != nullptr) {
        delete[] _location;
        _location = nullptr;
    }
}

void server::Set_content(std::string __content){
    _content = __content;
}

void server::Set_name(std::string __name){
    _name = __name;
}

void server::Set_host(std::string __host){
    _host = __host;
}

// void server::Set_port(std::vector<std::string> __port){
//     _port = __port;
// }

void server::Set_max_body_size(long long __max_body_size){
    _max_body_size = __max_body_size;
}

std::string server::Get_content(){
    return (_content);
}

std::string server::Get_name(){
    return(_name) ;
}

std::string server::Get_host(){
    return(_host) ;
}



// std::vector<std::string> server::Get_port(){
//     return(_port);
// }

long long server::Get_max_body_size(){
    return(_max_body_size);
}

int server::Get_number_of_location(){
    return(_number_of_location);
}

void server::Set_number_of_location(int __number_of_location){
    _number_of_location = __number_of_location;
}

void server::new_location(){
    _location = new location[_number_of_location];
}


std::vector<std::pair<std::string, int> > server::getSock()  {
    return _sock;
}

    // Setter for _sock
void server::setSock(std::string port,int sock) {
	for (size_t i = 0; i < _sock.size(); i++)
	{
		std::pair<std::string, int> tmp = _sock[i];
        if (tmp.first == port)
        {
            tmp.second = sock;
            break;
        }
	}
}




    // Getter for hints
struct addrinfo &server::getHints()  {
    return hints;
}

void server::setHints( struct addrinfo& newHints) {
    hints = newHints;
}

struct addrinfo* &server::getRes() {
    return res;
}

void server::setRes(struct addrinfo* newRes) {
    res = newRes;
}

struct addrinfo* &server::getP() {
    return p;
}

void server::setP(struct addrinfo* newP) {
    p = newP;
}

int server::getAddI()  {
    return addI;
}

void server::setAddI(int newAddI) {
    addI = newAddI;
}

std::vector<CGI> server::GetCgi(){
    return _CGI;
}

void  server::SetCgi(std::vector<CGI> __cgi){
    _CGI = __cgi;

}

int server::CheckNumberOfLocation(){
    std::string sentence = "location";
    int cont = 0;
    size_t pos = 0;

    while (((pos = _content.find(sentence, pos)) != (size_t)std::string::npos) && !sentence.empty()) {
            pos += sentence.length();
            cont++;
    }
    return cont;
}

void server::loadingErrorIndex(std::vector<std::string> lines, size_t &i){

    size_t found_at;
    std::string key;
    std::string value;
    std::map<std::string , std::string> er;
    std::map<std::string , std::string>::iterator it = er.begin();
    while (i++ < lines.size() -1) {
        if (lines[i].find("#") != std::string::npos || removeWhitespace(lines[i]).empty()) continue;
        found_at = lines[i].find(':');
        key = trim(lines[i].substr(0, found_at));
        value = trim(lines[i].substr(found_at + 1));
        if (found_at == std::string::npos) 
            continue;
        if (isValidEroorValue(key)){
            er.insert (it, std::pair<std::string , std::string>(key,value)); 
        }
        else
            break;
    }
    SetErr(er);
}

void server::LoidingAllowedMethods(std::vector<std::string> lines, size_t &i) {
    std::string tmp_;
    size_t found_at;
    std::string value;

    std::vector<std::string>  _allowed_methods;
    while (i++ < lines.size() -1) {
        if (lines[i].find("#") != std::string::npos || removeWhitespace(lines[i]).empty()) continue;
        found_at = lines[i].find('-');
        if (found_at == std::string::npos) 
            break;
        value = trim(lines[i].substr(found_at + 1));
        
        if (value == "GET" || value == "DELETE" || value == "POST")
            _allowed_methods.push_back(value);
        else
            break;
    }
    _location[_indixL].SetAllowed_methods(_allowed_methods);
}

void server::loadingLocationContent(std::vector<std::string> lines, size_t &i){
    size_t found_at;
    std::string key;
    std::string value;
    
    while(i++ < lines.size() -1) {
        if (lines[i].find("#") != std::string::npos || removeWhitespace(lines[i]).empty()) continue;
        found_at = lines[i].find(':');

        key = trim(lines[i].substr(0, found_at));
        value = trim(lines[i].substr(found_at + 1));

        if (key == "type") {
            if (hasSpace(value) || !startsWithSlash(value))
                throw std::runtime_error("Invalid type: " + value);
            _location[_indixL].SetType_of_location(value);
        }
        else if (key == "root_directory") {
            if (hasSpace(value))
                throw std::runtime_error("Invalid type: " + value);
            _location[_indixL].SetRoot_directory(value);
        }
        else if (key == "index") {
            _location[_indixL].GetIndex().push_back(value);
        }
        else if (key == "rewrite") {
            _location[_indixL].SetRewrite(value);
        }
        else if (key == "upload_dir") {
            _location[_indixL].SetUpload_dir(value);
        }
        else if (key == "allowed_methods") {
            LoidingAllowedMethods(lines , i);
            i--;
        }
        else if (key == "directoryListing"){
            if (value == "on")
                _location[_indixL].SetDirectoryListing(true);
            else
                _location[_indixL].SetDirectoryListing(false);   
        }
        else{
            _indixL ++;
            break;
        }
    }
}

void server::Getlocation(){
    for (int i = 0; i < _number_of_location; i++)
    {
        std::cout << "---------------------location  n "<< i <<" -----------------------" << std::endl;
        std::cout <<  "_type_of_location  : " <<_location[i].GetType_of_location() << std::endl;
        std::cout <<  " directoryListing  : " <<_location[i].GetDirectoryListing() << std::endl;
        std::cout <<  "_root_directory    : " <<_location[i].GetRoot_directory() << std::endl;
        std::cout <<  "uploiad_dir    : " <<_location[i].GetUpload_dir() << std::endl;

        std::cout <<  "rewrite  : " <<_location[i].GetRewrite() << std::endl;
        for (std::vector<std::string>::size_type y = 0; y < _location[i].GetIndex().size(); y++) {
            std::cout << " indix :  "<<_location[i].GetIndex()[y] << std::endl;
        }
        for (std::vector<std::string>::size_type y = 0; y < _location[i].GetAllowed_methods().size(); y++) {
            std::cout << " method :  "<<_location[i].GetAllowed_methods()[y] << std::endl;
        }

    }
}

void server::loadingCgiContent(std::vector<std::string> lines,size_t &i){
    size_t found_at;
    std::string key;
    std::string value;
    CGI tmp;

    while (i++ < lines.size() -1) {
        if (lines[i].find("#") != std::string::npos || removeWhitespace(lines[i]).empty()) continue;
        found_at = lines[i].find(':');
        key = trim(lines[i].substr(0, found_at));
        value = trim(lines[i].substr(found_at + 1));
        if (found_at == std::string::npos) 
            continue;
        if (key == "types"){
            if (value == "bash" || value == "PhP" || value == "python")
                 tmp.SetType(value);
            else
                throw std::runtime_error("Invalid script: " + trim(lines[i]));  
        }
        else if (key == "indix"){
            if (removeWhitespace(value).empty())
                throw std::runtime_error("enter indix for CGI : " + trim(lines[i]));  
            tmp.SetPath(value);
        }
        else
            break;
    }
    _CGI.push_back(tmp);
}    

void server::loadingDataserver(){
    int flage = 0;
    size_t found_at;
    std::string key;
    std::string value;
    std::vector<std::string> lines = StringToLines(_content);

    for (size_t i = 1; i < lines.size(); ++i) {
        if (lines[i].find("#") != std::string::npos || removeWhitespace(lines[i]).empty()) continue;

        found_at = lines[i].find(':');
        if (found_at == std::string::npos) 
            throw std::runtime_error("Invalid ppp: " + lines[i]);

        key = trim(lines[i].substr(0, found_at));
        value = trim(lines[i].substr(found_at + 1));
        
        CheckKey(key);  

        if (key == "name") {
            _name = value;
        }
        else if (key == "host") {
            _host = value;
            if (!isValidHost(_host)) {
                throw std::runtime_error("Invalid host: " + _host);
            }
        }
        else if (key == "port") {
            if (!isValidPort(value)) {
                throw std::runtime_error("Invalid port: " + value);
            }
            if (flage == 0){
                _sock[_NPort] = make_pair(value, 0);
                flage = 1;
            }
            else
                _sock.push_back(make_pair(value, 0));
            _NPort++;
        }
        else if (key == "max_body_size") {
            if (!parseBodySize(value, _max_body_size)) {
                throw std::runtime_error("Invalid body size: " + value);
            }
        }
        else if (key == "error_pages") {
            loadingErrorIndex(lines, i);
            i--;
        }
        else if (key == "location") {
            loadingLocationContent(lines, i);
            i--;
        }
        else if (key == "CGI") {
            loadingCgiContent(lines, i);
            i--;
        }
    }
    Getlocation();
}



//=================================


const char* server::InternalServerError::what() const throw()
{
    return("Saad Ka3i, 3lach??");
}

int server::run()
{
    struct addrinfo hints;
    struct addrinfo *res ,*p;
    int yes = 1;
    int addI;


    std::cout << "---------- CGI ---------------------------------------" << std::endl;
    for (std::vector<CGI>::size_type i = 0; i < _CGI.size(); i++)
    {
        std::cout << "the type is : " << _CGI[i].GetType() << std::endl;
        std::cout << "the indix is : " << _CGI[i].GetPath() << std::endl;

    }
    
    std::cout << "------------------------------------------------" << std::endl;


    for (std::vector<std::string>::size_type y = 0; y < _sock.size();  y++)
    {
        
        memset(&hints, 0,  sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        std::cout << "aaa chabab" << _sock[y].first.c_str() << std::endl;
        if ((addI = getaddrinfo(_host.c_str(), _sock[y].first.c_str(), &hints, &res)) != 0)
        {
            std::cerr << gai_strerror(addI) << std::endl;
            return -1;
        }

        for(p = res; p; p = p->ai_next)
        {
            if ((_sock[y].second = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
                continue;
            if ((setsockopt(_sock[y].second, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1)
            {
                perror("setsockopt");
                return (-1);
                /*exit(1);*/
            }
            //bind
            if (bind(_sock[y].second, res->ai_addr, res->ai_addrlen) == -1)
                continue;
            break;
        }

        freeaddrinfo(res); 
        if (!p)
        {
            perror("bind failed");
            return (-1);
            /*exit(1);*/
        }

        if (listen(_sock[y].second, 10) == -1)
        {
            perror("listen() failed");
            return (-1);
            /*exit(1);*/
        }
        /* code */
        std::cout << "Server is listening on " << _host << ":" << _sock[y].first << std::endl; 
    }
    

    return (0);
}

