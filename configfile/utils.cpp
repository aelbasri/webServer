/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zel-khad <zel-khad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 16:50:17 by zel-khad          #+#    #+#             */
/*   Updated: 2025/01/25 11:31:03 by zel-khad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

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

bool isValidEroorValue(const std::string& value) {
    for (size_t i = 0; i < value.length(); ++i) {
        if (!isdigit(value[i])) {
            return false;
        }
    }
    int portNum = atoi(value.c_str());
    if (portNum < 300 || portNum > 599)
        throw std::runtime_error("value of error must be between 300 and 599 : " + value);
    else 
        return true;

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

std::string removeWhitespace(const std::string input) {
    std::string result;
    
    for (size_t i = 0; i < input.length(); ++i) {
        if (!isspace(input[i])) {
            result += input[i];
        }
    }
    return result;
}

std::string escapeSpaces(const std::string input) {
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