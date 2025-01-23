/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_data.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:58:24 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/23 12:31:25 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_data.hpp"

server::server():_name("localhost"), _host("127.0.0.0"),_port("80"),_max_body_size(1048576){}

server::~server() {
    if (_location) {
        // delete[] _location;
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
    // _location = new location[_nembre_of_location];
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

std::string escapeSpaces(const std::string& input) {
    std::string result;
    bool insideQuotes = false;
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (c == '"') {
            insideQuotes = !insideQuotes;
            result += c; 
        }
        else if ((c == ' ' || c == '\t') && !insideQuotes) {
            continue; 
        }
        else {
            result += c;
        }
    }
    return result;
}


std::string trim(const std::string& input) {
    if (input.empty()) {
        return "";
    }
    
    size_t start = 0;
    size_t end = input.length() - 1;
    
    while (start < input.length() && isspace(input[start])) {
        ++start;
    }
    
    if (start == input.length()) {
        return "";
    }
    while (end > start && isspace(input[end])) {
        --end;
    }
    
    return input.substr(start, end - start + 1);
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
            markbegin = i + 1;
        }
    }
    if (markbegin < inputString.length()) {
        result.push_back(inputString.substr(markbegin));
    }
    return result;
}

bool parseBodySize(const std::string& sizeStr, long long& bytes) {
    if (sizeStr.empty()) return false;
    char unit;
    std::string numPart;
    if (!isdigit(sizeStr[sizeStr.length() - 1])){
        unit = sizeStr[sizeStr.length() - 1];
        numPart = sizeStr.substr(0, sizeStr.length() - 1);
    }
    else{
        unit = 'm';
        numPart = sizeStr.substr(0, sizeStr.length());
    }
    long long value = 0;
    for (size_t i = 0; i < numPart.length(); ++i) {
        if (!isdigit(numPart[i])) return false;
        value = value * 10 + (numPart[i] - '0');
    }
    if (sizeStr == "m"){
             bytes = 1048576;
            return (true);
    }
    else if (sizeStr == "k"){
            bytes = 1024;
            return (true);
    }
    else if (sizeStr == "g"){
          bytes = 1073741824;
        return (true);
    }
    switch (unit) {
        case 'm': 
            bytes = value * 1024 * 1024;
            return true;
        case 'k': 
            bytes = value * 1024;
            return true;
        case 'g': 
            bytes = value * 1024 * 1024 * 1024;
            return true;
        default:
            if (isdigit(unit)) {
                bytes = value;
                return true;
            }
            return false;
    }
}

bool isValidPort(const std::string& port) {
    for (size_t i = 0; i < port.length(); ++i) {
        if (!isdigit(port[i])) {
            return false;
        }
    }
    int portNum = atoi(port.c_str());
    return portNum > 0 && portNum <= 65535;
}

bool isValidHost(const std::string& host) {
    if (host.empty() || host.length() > 15) {
        return false;
    }

    int dots = 0;
    int value = 0;
    int segment = 0;
    
    for (size_t i = 0; i < host.length(); ++i) {
        char c = host[i];
        
        if (c == '.') {
            if (segment == 0 || i == host.length() - 1) {
                return false;
            }            
            if (value > 255) {
                return false;
            }
            value = 0;
            segment = 0;
            dots++;
            
            if (dots > 3) {
                return false;
            }
        }
        else if (isdigit(c)) {
            value = value * 10 + (c - '0');
            segment++;
            if (segment > 3) {
                return false;
            }
        }
        else {
            return false; 
        }
    }
    
    return dots == 3 && value <= 255 && segment > 0;
}

void server::loadingErrorIndex(std::vector<std::string> lines, int i){
        size_t found_at;

        while ( i < lines.size() ){
        if (lines[i].find("#") != string::npos){
            i++;
            continue;
        }
        if (lines[i].find("404") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing '404' formatted not as expected");
            }
            SetNotfound(lines[i].substr(found_at + 1));            
        }
        else if (lines[i].find("403") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing '403' formatted not as expected");
            }
            SetForbiden(lines[i].substr(found_at + 1));
        }
        else if (lines[i].find("405") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing '405' formatted not as expected");
            }
            SetMethodNotAllowed(lines[i].substr(found_at + 1));
        }
        else if (lines[i].find("default") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'default' formatted not as expected");
            }
            SetDefault(lines[i].substr(found_at + 1));
        }
        i++;
    }
}

void server::loadingDataserver(config_file *Conf){
        size_t found_at;
        std::string target;

        std::vector<std::string> lines = StringToLines(_content);

        for (std::vector<std::string>::size_type i = 0; i < lines.size(); ++i) {
        if (lines[i].find("#") != string::npos)
            continue;
        if (lines[i].find("name") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'name' formatted not as expected");
            }
            _name = lines[i].substr(found_at + 1);      
            _name = trim(_name);      
        } 
        else if (lines[i].find("host") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'host' formatted not as expected");
            }
            _host = lines[i].substr(found_at + 1);
            _host = trim(_host);
            if (isValidHost(_host) == false){
                throw runtime_error("line containing 'host' formatted not as expected : " + _host);
            } 
        }
        else if (lines[i].find("port") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'port' formatted not as expected");
            }
            _port = lines[i].substr(found_at + 1);
            _port = trim(_port);
            if (isValidPort(_port) == false){
                throw runtime_error("line containing 'port' formatted not as expected : " + _port);
            } 
        }
        else if (lines[i].find("max_body_size") != string::npos) {
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'max_body_size' formatted not as expected");
            }
            std::string tmp_ = lines[i].substr(found_at + 1);
            tmp_ = escapeSpaces(tmp_);
            if (parseBodySize( tmp_, _max_body_size) == false)
                throw runtime_error("line containing 'max_body_size' formatted not as expected : " + tmp_ );
                
        }
        else if (lines[i].find("error_pages") != string::npos){
            found_at = lines[i].find(':');
            if (found_at == string::npos) {
                throw runtime_error("line containing 'location' formatted not as expected");
            }
            loadingErrorIndex(lines , i);
        }
    }
}
