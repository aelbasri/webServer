/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:58:24 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/25 12:13:26 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_data.hpp"

server::server():_indixL(0),_location(nullptr),_name("localhost"), _host("127.0.0.0"),_port("80"),_max_body_size(1048576){}

server::~server() {
    if (_location) {
        delete[] _location;
        _location = NULL;
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

void server::Set_port(std::string __port){
    _port = __port;
}

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

std::string server::Get_port(){
    return(_port);
}

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


int server::getSock()  {
    return _sock;
}

    // Setter for _sock
void server::setSock(int sock) {
   _sock = sock;
}

    // Getter for hints
struct addrinfo &server::getHints()  {
    return hints;
}

    // Setter for hints
void server::setHints( struct addrinfo& newHints) {
    hints = newHints;
}

    // Getter for res
struct addrinfo* &server::getRes() {
    return res;
}

    // Setter for res
void server::setRes(struct addrinfo* newRes) {
    res = newRes;
}

    // Getter for p
struct addrinfo* &server::getP() {
    return p;
}

    // Setter for p
void server::setP(struct addrinfo* newP) {
    p = newP;
}

    // Getter for addI
int server::getAddI()  {
    return addI;
}

    // Setter for addI
void server::setAddI(int newAddI) {
    addI = newAddI;
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
            _location[_indixL].SetType_of_location(value);
        }
        else if (key == "root_directory") {
            _location[_indixL].SetRoot_directory(value);
        }
        else if (key == "index") {
            _location[_indixL].SetIndex(value);
        }
        else if (key == "allowed_methods") {
            LoidingAllowedMethods(lines , i);
            i--;
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
        // std::cout << "---------------------location  n "<< i <<" -----------------------" << std::endl;
        // std::cout <<  "_type_of_location  : " <<_location[i].GetType_of_location() << std::endl;
        // std::cout <<  "_index  : " <<_location[i].GetIndex() << std::endl;
        // std::cout <<  "_root_directory  : " <<_location[i].GetRoot_directory() << std::endl;
        // for (std::vector<std::string>::size_type y = 0; y < _location[i].GetAllowed_methods().size(); y++) {
        //     std::cout << " method :  "<<_location[i].GetAllowed_methods()[y] << std::endl;
        // }   
    }
}

void server::loadingDataserver(){
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
            _port = value;
            if (!isValidPort(_port)) {
                throw std::runtime_error("Invalid port: " + _port);
            }
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

    memset(&hints, 0,  sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((addI = getaddrinfo(_host.c_str(), _port.c_str(), &hints, &res)) != 0)
    {
        std::cerr << gai_strerror(addI) << std::endl;
        return -1;
    }

    for(p = res; p; p = p->ai_next)
    {
        if ((_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
            continue;
        if ((setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1)
        {
            perror("setsockopt");
            return (-1);
            /*exit(1);*/
        }
        //bind
        if (bind(_sock, res->ai_addr, res->ai_addrlen) == -1)
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

    if (listen(_sock, 10) == -1)
    {
        perror("listen() failed");
        return (-1);
        /*exit(1);*/
    }

    std::cout << "Server is listening on " << _host << ":" << _port << std::endl; 
    return (0);
}

// void send_res(Request &request, int new_fd)
// {
//     Response response;
//     std::string path("./assets");
    
//     if (request.getRequestTarget() == "/")
//         path += "/index.html";
//     else
//         path += request.getRequestTarget();
//     std::ifstream file(path.c_str());

//     std::cout << "PATH: "  << path << std::endl;

//     if (request.getRequestTarget() == "/redirection")
//     {
// 	std::string location = "https://www.youtube.com/watch?v=vmDIwhP6Q18&list=RDQn-UcLOWOdM&index=2";

//         std::cout << "Ridddddaryrikchn" << std::endl;
//         path = "./assets/302.html";
//         std::string connection = "close";
//         std::string contentType = getMimeType(path);

//         response.setHttpVersion(HTTP_VERSION);
//         response.setStatusCode(302);
//         response.setReasonPhrase("Moved Temporarily");
//         response.setFile(path);

//         response.setContentLength();
//         response.addHeader(std::string("Location"), location);
//         response.addHeader(std::string("Content-Type"), contentType);
//         response.addHeader(std::string("Connection"), connection);
//         /*response.sendResponse(new_fd);*/
//         int sent = 0;
//         while (!sent)
//             sent = response.sendResponse(new_fd);
//     }
//     else if (!file.good()) {
//         std::cout << "iror nat found" << std::endl;
//         path = "./assets/404.html";
//         std::string connection = "close";
//         std::string contentType = getMimeType(path);

//         response.setHttpVersion(HTTP_VERSION);
//         response.setStatusCode(404);
//         response.setReasonPhrase("Not Found");
//         response.setFile(path);

//         response.setContentLength();
//         response.addHeader(std::string("Content-Type"), contentType);
//         response.addHeader(std::string("Connection"), connection);
//         /*response.sendResponse(new_fd);*/
//         int sent = 0;
//         while (!sent)
//             sent = response.sendResponse(new_fd);
//     }
//     else
//     {
//         std::cout << "saad t3asb "  << path << std::endl;

//         std::string connection = "close";
//         std::string contentType = getMimeType(path);

//         response.setHttpVersion(HTTP_VERSION);
//         response.setStatusCode(200);
//         response.setReasonPhrase("OK");
//         response.setFile(path);

//         response.setContentLength();
//         response.addHeader(std::string("Content-Type"), contentType);
//         response.addHeader(std::string("Connection"), connection);
//         /*response.sendResponse(new_fd);*/
//         int sent = 0;
//         while (!sent)
//             sent = response.sendResponse(new_fd);
//     }
// }

// void handle_request(int new_fd)
// {
//     /*Parse Request*/
//     try
//     {
//         Request request;
//         int offset = 0;
//         int nBytes = 0;
//         enum State myState = REQUEST_LINE;


//         while(myState != DONE)
//         {
//             switch (myState)
//             {
//                 case  REQUEST_LINE :
//                     request.parseRequestLine(new_fd, offset, nBytes);
//                     myState = HEADER;
//                     break;
//                 case HEADER :
//                     request.parseHeader(new_fd, offset, nBytes);
//                     myState = BODY;
//                     break;
//                 case BODY :
//                     request.parseBody(new_fd, offset, nBytes);
//                     myState = DONE;
//                     break;
//                 default:
//                     break;
//             }
//         }
//         //throw Server::InternalServerError();
//         send_res(request, new_fd);
//     }
//     catch(const std::exception& e)
//     {
//         Response response;
//         std::cout << "Saad 500Error" << std::endl;
//         std::string path = "./assets/50x.html";
//         std::string connection = "close";
//         std::string contentType = getMimeType(path);

//         response.setHttpVersion(HTTP_VERSION);
//         response.setStatusCode(500);
//         response.setReasonPhrase("Internal Server Error");
//         response.setFile(path);

//         response.setContentLength();
//         response.addHeader(std::string("Content-Type"), contentType);
//         response.addHeader(std::string("Connection"), connection);
//         /*response.sendResponse(new_fd);*/
//         int sent = 0;
//         while (!sent)
//             sent = response.sendResponse(new_fd);
//         // std::cerr << e.what() << std::endl;
//     }
// }
