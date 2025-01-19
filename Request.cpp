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

    // std::cout << "{" << method  << "}" << std::endl;
    // std::cout << "{" << requestTarget  << "}" << std::endl;
    // std::cout << "{" << httpVersion  << "}" << std::endl;
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
    return(0);
}

//parse body

int get_length(char *buffer, int &offset, int &nBytes)
{
    std::string s_number = "";
    int start = 0;
    while (1)
    {
        if ((offset + start + 1) >= nBytes || ((buffer[offset + start] == '\r') && (buffer[offset + start + 1] == '\n')))
            break;
        s_number += buffer[offset + (start++)];
    }
    offset += start + 2;
    std::cout << "Length s: " << s_number << std::endl;
    int length;
    std::stringstream ss;
    ss << std::hex << s_number;
    ss >> length;
    return (length);
}

bool skipHeader(int count, int &offset, char *buffer, int nBytes)
{
    while (offset < nBytes)
    {
        if ((offset - 1) >= 0  && buffer[offset] == '\n' && buffer[offset - 1] == '\r')
        {
            if (count == 2)
                return (offset++, false);
            count = 0;
        }
        count++;
        offset++;
    }
    return (true);
}

int getChunkSize(int &offset, char *buffer, int &nBytes)
{
    std::string s_number = "";
    while (1)
    {
        if (offset + 1 >= nBytes || ((buffer[offset] == '\r') && (buffer[offset + 1] == '\n')))
            break;
        s_number += buffer[offset++];
    }
    offset += 2;
    std::cout << "Length s: " << s_number << std::endl;
    int chunkSize;
    std::stringstream ss;
    ss << std::hex << s_number;
    ss >> chunkSize;

    return (chunkSize);
}


void loadChunkBoundry(parseBodyElement &body,std::string &boundaryCmp, std::string &boundary)
{
    int i = body.offset;
    
    while (i < body.nBytes)
    {
        boundaryCmp += body.buffer[i];
        if ((i - 1) >= 0  && body.buffer[i] == '\n' && body.buffer[i - 1] == '\r')
        {
            if(boundaryCmp == boundary)
            {
                if (i >= (int)(boundary.size() + 1))
                    body.nBytes = i - (boundary.size() + 1);
                boundaryCmp.clear();
                break;
            }
            boundaryCmp.clear();
        }
        i++;
    }
    body.consumed += body.nBytes - body.offset;
    body.file.write(body.buffer + body.offset, body.nBytes - body.offset);
    body.offset += body.nBytes;
}

void loadChunk(parseBodyElement &body, int &chunkSize)
{
    int toBeConsumed = 0;

    if (body.consumed < chunkSize)
    {
        toBeConsumed = chunkSize - body.consumed;
        if (toBeConsumed > body.nBytes - body.offset)
            toBeConsumed = body.nBytes - body.offset;
        
        body.file.write(body.buffer + body.offset, toBeConsumed);
        body.consumed = toBeConsumed;
        body.offset += body.consumed; 
    }
    if (body.consumed >= chunkSize)
        chunkSize = -1;
}

int Request::parseBody(int socket, int &offset, int &nBytes)
{
    parseBodyElement body;

    body.offset = ++offset;
    body.buffer = buffer;
    body.consumed = 0;
    body.nBytes = nBytes;
    body.file.open("out.jpg", std::ios::binary);

    fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

    if (header.find("Content-Length") != header.end())
    {
        std::string boundary("");
        size_t pos = header["Content-Type"].find("boundary");
        if (pos != std::string::npos)
        {
            boundary = header["Content-Type"].substr(pos + 9); // "boundary=".size() = 9
            boundary.insert(0, "--");
            boundary += "\r\n";
        }
        long contentLen = strtol(header["Content-Length"].c_str(), NULL, 10);

        bool flag = false;
        int count = 0;
        std::string boundaryCmp = boundary;

        while (body.consumed < contentLen)
        {
            if((body.offset >= body.nBytes))
            {
                if ((body.nBytes = recv(socket, body.buffer, BUFF_SIZE - 1, 0)) <= 0)
                    break;
                else
                    body.offset = 0;   
            }
            if ((boundary.size() > 0 && boundaryCmp == boundary) || flag)
            {
                flag = skipHeader(count, body.offset, body.buffer, body.nBytes);
                boundaryCmp.clear();
            }
            loadChunkBoundry(body, boundaryCmp, boundary);
        }
        body.file.close();

    }
    else if(header["Transfer-Encoding"] == "chunked")
    {
        int chunkSize = -1;

        while (1)
        {
            if((body.offset >= body.nBytes))
            {
                if ((body.nBytes = recv(socket, body.buffer, BUFF_SIZE - 1, 0)) <= 0)
                    break;
                else
                    body.offset = 0;   
            }
            if(chunkSize == -1)
                chunkSize = getChunkSize(body.offset, body.buffer ,body.nBytes);
            loadChunk(body, chunkSize);
        }
    }
    body.file.close();
    return (0);
}


