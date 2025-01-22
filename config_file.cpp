/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:21 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 10:00:20 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

using namespace std;

location::location(){
    std::cout << " ----------------constru " << std::endl;
}

location::~location(){
    cout << "destructer location is caled" << endl;
}

error_pages::error_pages(){}

error_pages::~error_pages(){}

server::server():_name("localhost"), _host("127.0.0.0"),_port("80"),_max_body_size("1m"){}

server::~server(){
    delete [] _location;
}

void err(){
    std::cerr << "error " << std::endl;
    exit(1);
}


void server::Set_content(std::string __content){
    _content = __content;
}

void server::Set_name(std::string __name){
    _name = __name;
}

void server::Set_host(std::string __host){
    _host == __host;
}

void server::Set_port(std::string __port){
    _port == __port;
}

void server::Set_max_body_size(std::string __max_body_size){
    _max_body_size == __max_body_size;
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

void config_file::getServer(){
    for (size_t i = 0; i < _nembre_of_server; i++)
    {
        std::cout << "---------------------------------sever n "<< i <<" ------------------------------------" << std::endl;
       std::cout << _server[i].Get_content() << std::endl;

       std::cout << "nembr of location is  :   " << _server[i].Get_nembre_of_location() << std::endl;

       
       std::cout << "def host  :   " << _server[i].Get_host() << std::endl;
       std::cout << "def name  :   " << _server[i].Get_name() << std::endl;
       std::cout << "def max budy  :   " << _server[i].Get_max_body_size() << std::endl;
       std::cout << "def port  :   " << _server[i].Get_port() << std::endl;
    }
    
}

void config_file::loadContentServer() {
    std::string sentence = "server:";
    size_t pos = 0;
    
    for (size_t i = 0; i < _nembre_of_server; i++) {
        pos = _fileContent.find(sentence, pos);
        if (pos == std::string::npos)
            break;
        
        pos += sentence.length();
        size_t nextServerPos = _fileContent.find(sentence, pos);        
        std::string serverContent;
        if (nextServerPos != std::string::npos)
            serverContent = _fileContent.substr(pos, nextServerPos - pos);
        else 
            serverContent = _fileContent.substr(pos);
        
        _server[i].Set_content(serverContent);        
        if (nextServerPos != std::string::npos)
            pos = nextServerPos;
       _server[i].Set_nembre_of_location(_server[i].CheckNumberOfLocation());
       _server[i].new_location();
    }
    
}

void loidingFile(config_file *Conf){
    string s;
    std::string strCRLF = "\r\n";

    ifstream f("config_file.yaml");

    if (!f.is_open()) {
        err();
    }
    
    while (getline(f, s)){
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace),
        s.end());
        Conf->setFileContent() += s;
        if (!s.empty())
            Conf->setFileContent().push_back('\n');
    }
    f.close();
}


config_file::config_file(){
    loidingFile(this);
    _nembre_of_server = CheckNumberOfServer();
    _server =  new server[_nembre_of_server];
    
}


int config_file::get_nembre_of_server(){
    return(_nembre_of_server);
}

config_file::~config_file(){
    delete[] _server;
}

std::string& config_file::setFileContent(){
    return(_fileContent);
}


int config_file::CheckNumberOfServer(){
    std::string sentence = "server:";
    int words = 0;
    size_t pos = 0;
    int lenOfSentence = sentence.size();
    while (((pos = _fileContent.find(sentence, pos)) != (size_t)std::string::npos) && !sentence.empty()) {
            pos += sentence.length();
            words++;
    }
    return words;
}


void server::loadingDataserver(config_file Conf){
    std::cout << Conf.setFileContent()<< std::endl;
}


int main() {

    config_file Conf; 
    server *server;

    Conf.loadContentServer();
    Conf.getServer();
    

    
    return 0;
}