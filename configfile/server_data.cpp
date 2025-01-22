/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:58:24 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/22 14:55:00 by zel-khad         ###   ########.fr       */
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

std::vector<std::string> StringToLines(const std::string& inputString) {
    std::vector<std::string> result;
    std::string temp;
    int markbegin = 0;
    int markend = 0;

    for (int i = 0; i < inputString.length(); ++i) {     
        if (inputString[i] == '\n') {
            markend = i;
            result.push_back(inputString.substr(markbegin, markend - markbegin));
            markbegin = i + 1;  // Skip the newline character
        }
    }
    if (markbegin < inputString.length()) {
        result.push_back(inputString.substr(markbegin));
    }

    return result;
}

void server::loadingDataserver(config_file *Conf){
        int i = 0;

        size_t found_at;
        int start;


        
        std::vector<std::string> lines = StringToLines(_content);

        for (std::vector<std::string>::size_type i = 0; i < lines.size(); ++i) {
        // if (lines[i].find("host") != string::npos) {
        //     found_at = lines[i].find(':');
            cout << "================|"<< lines[i]<<"|===========================" << endl;
            // // while (std::isprint(_content[i]))
            // // {
            // //     cout << "ha ana ---===---==--=" << endl;
            // // }
            
            // if (found_at == string::npos) {
            //     throw runtime_error("line containing 'BEST_KNOWN' formatted not as expected");
            // }
            // _name = lines[i].substr(found_at + 1);
            // break;
            
        // } 
        // else if (_content.find("host") != string::npos) {
        //     found_at = _content.find(':');

        //     if (found_at == string::npos) {
        //         throw runtime_error("line containing 'DIMENSION' formatted not as expected");
        //     }

        //     _host.substr(found_at + 1);
        //     break;
            
        // } 
        // else if (line.find("NODE_COORD_SECTION") != string::npos) {

        //     if (dimension == -1 || best_known == -1) {
        //         throw runtime_error("dimension and best known result should have already been read");
        //     }

        //     unsigned index;
        //     double x, y;

        //     while (file >> index >> x >> y) {
        //         unique_ptr <Node> p(new Node(index, x, y));
        //         nodes.push_back(move(p));
        //     }

        //     break;
        // }
        // i++;
    }
    // std::cout << Conf->setFileContent()<< std::endl;
}
