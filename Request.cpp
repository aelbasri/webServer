#include "Request.hpp"

int temporaryPrintError()
{
    //throw BadRequestExeption
    perror("Bad request");
    return (-1);
}

bool isWhiteSpace(char c)
{
    if ((c >= 9 && c <= 13) || c == 32)
        return (true);
    return (false);
}

bool parseField(std::string field, std::string &fieldName, std::string &fieldValue)
{
    size_t len = field.size();
    size_t i = 0;
    while (i < len && field[i] != ':')
    {
        if (isWhiteSpace(field[i]))
            return (false);
        i++;
    }

    fieldName = field.substr(0, i);
    fieldValue = field.substr(i + 1, len - i);

    int valueLen = fieldValue.size();
    if (isWhiteSpace(fieldValue[0]))
        fieldValue.erase(0, 1);
    if (isWhiteSpace(fieldValue[valueLen - 1]))
        fieldValue.erase(valueLen - 1); 
    valueLen = fieldValue.size();
    if (isWhiteSpace(fieldValue[0]) || isWhiteSpace(fieldValue[valueLen - 1]))
        return (false);
    if (fieldName.empty() || fieldValue.empty())
        return (false);
    return(true);
}

int Request::parseRequestLine(int socket, int &offset, int &nBytes)
{
    std::string firstLine("");
    std::vector<std::string> elements;

    //Loop reach '\n'
    while ((nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) > 0)
    {
        write(1, buffer , nBytes);
        std::cout << "===> bytes received: " << nBytes << std::endl;
        offset = 0;
        while (offset < nBytes && buffer[offset] != '\n')
            firstLine += buffer[offset++];
        if(buffer[offset] == '\n')
            break;
    }
    
    // std::cout << "===> daz mn hna " << nBytes << std::endl;
    if(firstLine[offset - 1] != '\r' || firstLine.empty())
        return (temporaryPrintError());
    int start = 0;
    int i = 0;
    for(; i < offset - 1; i++)
    {
        if (isWhiteSpace(firstLine[start]))
            return (temporaryPrintError());
        if (isWhiteSpace(firstLine[i]))
        {
            elements.push_back(firstLine.substr(start, i - start));
            start = i + 1;
        }
    }
    if (i > 0 && isWhiteSpace(firstLine[i - 1]))
        return (temporaryPrintError());
    else
        elements.push_back(firstLine.substr(start, i - start));
    if (elements.size() != 3)
        return (temporaryPrintError());
    else
    {
        method = elements[0];
        requestTarget = elements[1];
        httpVersion = elements[2];
    }
    offset++;

    std::cout << "{" << method  << "}" << std::endl;
    std::cout << "{" << requestTarget  << "}" << std::endl;
    std::cout << "{" << httpVersion  << "}" << std::endl;
    return (0);
}

int Request::parseHeader(int socket, int &offset, int &nBytes)
{
    std::string field("");

    while(offset < nBytes || (offset = 0)|| (nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) > 0)
    {
        if (buffer[offset] == '\n' && offset - 1 >= 0 && buffer[offset - 1] == '\r')
        {
            field.erase(field.end() - 1);
            if(field.empty())
                break;
            std::string fieldName;
            std::string fieldValue;
            if(!parseField(field, fieldName, fieldValue))
                return(temporaryPrintError());
            header.insert(std::make_pair(fieldName, fieldValue));
            field.clear();
        }
        else 
            field += buffer[offset];
        offset++;

    }
    // std::cout << "A=====" << std::endl; 
    // write(1, buffer, (nBytes));
    // std::cout << "\n=====Z" << std::endl; 
    for(std::map<std::string, std::string>::iterator i = header.begin(); i != header.end(); i++)
    {
        std::cout << "{" << i->first << "}" << ":" << "{" << i->second << "}" << std::endl;
    }
    return(0);
}

int Request::parseBody(int socket, int &offset, int &nBytes)
{
    std::ofstream file("out.txt", std::ios::binary);
    fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    if (offset < nBytes)
        file.write(buffer + offset + 1, nBytes - (offset + 1));
    if (header.find("Content-Length") != header.end())
    {
        // parse if there is a header 
        long contentLen = strtol(header["Content-Length"].c_str(), NULL, 10);
        int count = nBytes - (offset + 1);
        int qraya = 0;
        while(contentLen > count)
        {
            if((nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) <= 0)
                break;
            count += nBytes;
            file.write(buffer, nBytes);
            std::cout << ++qraya << std::endl;
        }
    }
    else if(header["Transfer-Encoding"] == "chunked")
    {
        std::cout << "yoyoyoyoyo"  << std::endl;
        while(1)
        {
            if((nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) <= 0)
                break;
            // count += nBytes;
            file.write(buffer, nBytes);
        }
    }
    // std::cout << "}}}}>" << count << std::endl;
    return (0);
}
