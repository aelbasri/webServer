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

server::server():_location(NULL),_indixL(0),_name("localhost"), _host("127.0.0.0"),_port("80"),_max_body_size(1048576){}

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

int server::Get_nembre_of_location(){
    return(_nembre_of_location);
}

void server::Set_nembre_of_location(int __nembre_of_location){
    _nembre_of_location = __nembre_of_location;
}

void server::new_location(){
    _location = new location[_nembre_of_location];
}

int server::CheckNumberOfLocation(){
    std::string sentence = "location";
    int cont = 0;
    size_t pos = 0;
    int lenOfSentence = sentence.size();
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
    map<std::string , std::string> er;
    std::map<std::string , std::string>::iterator it = er.begin();
    while (i++ < lines.size()) {
        size_t found_at = lines[i].find(':');
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
    while (i++ < lines.size()) {
        if (lines[i].find("#") != std::string::npos) 
            continue;
        found_at = lines[i].find('-');
        if (found_at == std::string::npos) 
            break;
        value = trim(lines[i].substr(found_at + 1));
        
        if (value == "GET" || "DELETE" || "POST")
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
    
    while(i++ < lines.size() ) {
        if (lines[i].find("#") != std::string::npos) 
            continue;
        found_at = lines[i].find(':');
        // if (found_at == std::string::npos) 
        //     throw std::runtime_error("Invalid key: " + lines[i]);
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
    // std::string<>
    for (size_t i = 0; i < _nembre_of_location; i++)
    {
        std::cout << "---------------------location  n "<< i <<" -----------------------" << std::endl;
        std::cout <<  "_type_of_location  : " <<_location[i].GetType_of_location() << std::endl;
        std::cout <<  "_index  : " <<_location[i].GetIndex() << std::endl;
        std::cout <<  "_root_directory  : " <<_location[i].GetRoot_directory() << std::endl;
        for (std::vector<std::string>::size_type y = 0; y < _location[i].GetAllowed_methods().size(); y++) {
            std::cout << " method :  "<<_location[i].GetAllowed_methods()[y] << std::endl;
        }
        
    }
    
}

void server::loadingDataserver(config_file *Conf){
        int flage = 0;
        size_t found_at;
        std::string key;
        std::string value;
        std::vector<std::string> lines = StringToLines(_content);

        for (size_t i = 0; i < lines.size(); ++i) {
            // throw std::runtime_error("Invalid KEY: " + lines[i]);

        if (lines[i].find("#") != std::string::npos) 
            continue;
        // if ()
        found_at = lines[i].find(':');
        // if (found_at == std::string::npos) 
        //     break;;

        key = trim(lines[i].substr(0, found_at));
        value = trim(lines[i].substr(found_at + 1));

        if (key == "id")
            continue;

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
        // else
        //     throw std::runtime_error("Invalid KEY: " + lines[i]);
            
    }
    Getlocation();
}

