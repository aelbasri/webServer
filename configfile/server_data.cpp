/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:58:24 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 11:39:50 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_data.hpp"

server::server():_name("localhost"), _host("127.0.0.0"),_port("80"),_max_body_size("1m"){}

server::~server(){
    delete [] _location;
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

void server::Set_max_body_size(std::string __max_body_size){
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

std::string server::Get_max_body_size(){
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

void server::loadingDataserver(config_file Conf){
    std::cout << Conf.setFileContent()<< std::endl;
}
