/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:45:21 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/20 17:52:30 by zel-khad         ###   ########.fr       */
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

config_file::config_file(){}

config_file::~config_file(){}

std::string& config_file::setFileContent(){
    return(_fileContent);
}

void err(){
    std::cerr << "error " << std::endl;
    exit(1);
}

void loidingFile(config_file *Conf){
    string s;
    ifstream f("config_file.yaml");

    if (!f.is_open()) {
        err();
    }
    
    while (getline(f, s)){
        Conf->setFileContent() += s;
        Conf->setFileContent().push_back('\n');
    }
    f.close();
}

int main() {

    config_file Conf;
    loidingFile(&Conf);
    std::cout << Conf.setFileContent()<< std::endl;
    
    return 0;
}