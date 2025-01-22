/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:21 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 23:08:27 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

void err(){
    std::cerr << "error " << std::endl;
    exit(1);
}


void config_file::getServer(){
    for (size_t i = 0; i < _nembre_of_server; i++)
    {
        
        std::cout << "---------------------------------sever n "<< i <<" ------------------------------------" << std::endl;
    //    std::cout << _server[i].Get_content() << std::endl;

       std::cout << "nembr of location is  :   " << _server[i].Get_nembre_of_location() << std::endl;

       
       std::cout << "def host  :   " << _server[i].Get_host() << std::endl;
       std::cout << "def name  :   " << _server[i].Get_name() << std::endl;
       std::cout << "def max budy  :   " << _server[i].Get_max_body_size() << std::endl;
       std::cout << "def port  :   " << _server[i].Get_port() << std::endl;
       
       std::cout <<  "_Forbidden  : " <<_server[i].GetForbiddent() << std::endl;
       std::cout <<  "_NotFound  : " <<_server[i].GetNotFound() << std::endl;
       std::cout <<  "_MethodNotAllowed  : " <<_server[i].GetMethodNotAllowed() << std::endl;
       std::cout <<  "_default  : " <<_server[i].GetDefault() << std::endl;

    }
    
}

void config_file::loadContentServer() {
    std::string sentence = "id";
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
        _server[i].loadingDataserver(this);
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
    if (_server){
        delete[] _server;
        _server = NULL;
    }
}

std::string& config_file::setFileContent(){
    return(_fileContent);
}


int config_file::CheckNumberOfServer(){
    std::string sentence = "id";
    int words = 0;
    size_t pos = 0;
    int lenOfSentence = sentence.size();
    while (((pos = _fileContent.find(sentence, pos)) != (size_t)std::string::npos) && !sentence.empty()) {
            pos += sentence.length();
            words++;
    }
    return words;
}

int main() {
    try
    {
        config_file Conf; 
        Conf.loadContentServer();
        Conf.getServer();

     }
     catch (runtime_error e)
     {
         cout << "L***A JAAAY!: " << e.what();
     }

    return 0;
}