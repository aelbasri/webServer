/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:21 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/21 13:06:49 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

using namespace std;

location::location(){}

location::~location(){}

error_pages::error_pages(){}

error_pages::~error_pages(){}

server::server(){}

server::~server(){}

void err(){
    std::cerr << "error " << std::endl;
    exit(1);
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

    std::cout << Conf.get_nembre_of_server() << std::endl;
    // Conf.loadingDataserver(Conf);
    
    return 0;
}