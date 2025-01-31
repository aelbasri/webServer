#include "Request.hpp"
#include "Response.hpp"

std::string Request::getRequestTarget(void) const
{
    return(requestTarget);
}
std::string Request::getMethod(void) const
{
    return(method);
}
std::string Request::getHttpVersion(void) const
{
    return (httpVersion);
}

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


// int Request::parseRequestLine(int socket, int &offset, int &nBytes)
// {
//     std::string firstLine("");
//     std::vector<std::string> elements;

//     //Loop reach '\n'
//     while ((nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) > 0)
//     {
//         write(1, buffer , nBytes);
//         offset = 0;
//         while (offset < nBytes && buffer[offset] != '\n')
//             firstLine += buffer[offset++];
//         if(buffer[offset] == '\n')
//             break;
//     }
    
//     if(firstLine[offset - 1] != '\r' || firstLine.empty())
//         return (temporaryPrintError());
//     int start = 0;
//     int i = 0;
//     for(; i < offset - 1; i++)
//     {
//         if (isWhiteSpace(firstLine[start]))
//             return (temporaryPrintError());
//         if (isWhiteSpace(firstLine[i]))
//         {
//             elements.push_back(firstLine.substr(start, i - start));
//             start = i + 1;
//         }
//     }
//     if (i > 0 && isWhiteSpace(firstLine[i - 1]))
//         return (temporaryPrintError());
//     else
//         elements.push_back(firstLine.substr(start, i - start));
//     if (elements.size() != 3)
//         return (temporaryPrintError());
//     else
//     {
//         method = elements[0];
//         requestTarget = elements[1];
//         httpVersion = elements[2];
//     }
//     offset++;

//     // parseMethod();
//     // parseRequestTarget();
//     // parseHttpVersion();
//     return (0);
// }

// int Request::parseHeader(int socket, int &offset, int &nBytes)
// {
//     std::string field("");

//     while(offset < nBytes || (offset = 0)|| (nBytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) > 0)
//     {
//         if (buffer[offset] == '\n' && offset - 1 >= 0 && buffer[offset - 1] == '\r')
//         {
//             field.erase(field.end() - 1);
//             if(field.empty())
//                 break;
//             std::string fieldName;
//             std::string fieldValue;
//             if(!parseField(field, fieldName, fieldValue))
//                 return(temporaryPrintError());
//             header.insert(std::make_pair(fieldName, fieldValue));
//             field.clear();
//         }
//         else 
//             field += buffer[offset];
//         offset++;

//     }
//     return(0);
// }

//parse body


bool skipHeader(int &count, int &offset, char *buffer, int nBytes)
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

int getChunkSize(int &offset, char *buffer, int &nBytes, std::string &s_number)
{
    int start = 0;
    while (1)
    {
        if ((offset + start) > nBytes)
        {
            offset = nBytes;
            return(-1);
        }
        if ((offset + start + 1) > nBytes)
        {
            offset += start;
            return (-1);
        }
        if ((buffer[offset + start] == '\r') && (buffer[offset + start + 1] == '\n'))
        {
            if(s_number.empty())
            {
                offset += start + 2;
                return(-1);
            }
            break;
        }
        s_number += buffer[offset + (start++)];
    }
    offset += start + 2;
    offset = std::min(offset, nBytes);
    int length;
    std::stringstream ss;
    ss << std::hex << s_number;
    ss >> length;
    s_number = "";
    return (length);
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
        body.consumed += toBeConsumed;
        body.offset += toBeConsumed; 
    }
    if (body.consumed >= chunkSize)
    {
        body.offset += 2;
        body.consumed = 0;
        chunkSize = -1;
    }
}

// int Request::parseBody(int socket, int &offset, int &nBytes)
// {
//     parseBodyElement body;

//     body.offset = ++offset;
//     body.buffer = buffer;
//     body.consumed = 0;
//     body.nBytes = nBytes;
//     body.file.open("out.jpg", std::ios::binary);

//     fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

//     if(headers["Transfer-Encoding"] == "chunked")
//     {
//         int chunkSize = -1;
//         std::string s_number = "";
//         while (1)
//         {
//             if((body.offset >= body.nBytes))
//             {
//                 if ((body.nBytes = recv(socket, body.buffer, BUFF_SIZE, 0)) <= 0)
//                     break;
//                 else
//                     body.offset = 0;   
//             }
//             if(chunkSize == -1)
//                 chunkSize = getChunkSize(body.offset, body.buffer ,body.nBytes, s_number);
//             if (chunkSize == 0)
//                 break;
//             loadChunk(body, chunkSize);
//         }
//     }
//     else if (headers.find("Content-Length") != headers.end())
//     {
//         int contentLen = strtol(headers["Content-Length"].c_str(), NULL, 10);

//         if (body.offset < body.nBytes)
//         {
//             body.file.write(body.buffer + body.offset, body.nBytes - body.offset);
//             body.consumed = body.nBytes - body.offset;
//         }
//         while (body.consumed < contentLen)
//         {
//             if ((body.nBytes = recv(socket, body.buffer, BUFF_SIZE, 0)) <= 0)
//                     break;
//             int toBeConsumed = std::min(body.nBytes, contentLen - body.consumed);
//             body.file.write(body.buffer, toBeConsumed);
//             body.consumed += toBeConsumed;
//         }
//     }
//     else if (headers.find("Content-Length") != headers.end() && (headers["Content-Type"].find("boundary") != std::string::npos))
//     {
//     }
//     body.file.close();
//     return (0);
// }


void Request::parseRequestLine(char *buffer, int i)
{
    switch (subState)
    {
        case METHOD :
            if (buffer[i] == ' ')
            {
                if (method != "GET" && method != "POST" && method != "DELETE")
                    throw Request::badRequest();
                subState = REQUEST_TARGET;
                break;
            }
            if (method.empty())
            {
                if (buffer[i] == 'G')
                    method += buffer[i];
                else if (buffer[i] == 'P')
                    method += buffer[i];
                else if (buffer[i] == 'D')
                    method += buffer[i];
                else
                    throw Request::badRequest();
                indexMethod++;
            }
            else if (method[0] == 'G')
            {
                method += buffer[i];
                if (method[indexMethod] != "GET"[indexMethod])
                    throw badRequest();
                indexMethod++;
            }
            else if (method[0] == 'P')
            {
                method += buffer[i];
                if (method[indexMethod] != "POST"[indexMethod])
                    throw badRequest();
                indexMethod++;
            }
            else if (method[0] == 'D')
            {
                method += buffer[i];
                if (method[indexMethod] != "DELETE"[indexMethod])
                    throw badRequest();
                indexMethod++;
            }
            break;
        case REQUEST_TARGET :
            // origin-form = absolute-path [ "?" query ]
            if (requestTarget.empty() && buffer[i] != '/')
                throw badRequest();
            if (buffer[i] != ' ' && buffer[i] != '?')
                requestTarget += buffer[i];
            else if (buffer[i] == '?')
                subState = QUERY_KEY;
            else
            {
                if (requestTarget.empty())
                    throw badRequest();
                subState = HTTP_VERSION_;
            }
            break;
        case QUERY_KEY:
            if (buffer[i] == '=')
            {
                if (queryName.empty())
                    throw badRequest();
                query[queryName] = "";
                subState = QUERY_VALUE;
            }
            else if (buffer[i] == ' ' || buffer[i] == '&')
                throw badRequest();
            else
                queryName += buffer[i];
            break;
        case QUERY_VALUE:
            if (buffer[i] == '&')
            {
                if (queryName.empty() || queryValue.empty())
                    throw badRequest();
                query[queryName] = queryValue;
                subState = QUERY_KEY;
                queryName = "";
                queryValue = "";
            }
            else if (buffer[i] == ' ')
            {
                if (queryValue.empty())
                    throw badRequest();
                query[queryName] = queryValue;
                subState = HTTP_VERSION_;
            }
            else
                queryName += buffer[i];
            break;
        case HTTP_VERSION_:
            if (buffer[i] != "HTTP"[indexHttp])
                    throw badRequest();
            if (buffer[i] == 'P')
                subState = FORWARD_SKASH;
            httpVersion += buffer[i];
            indexHttp++;
            break;
        case FORWARD_SKASH:
            if (buffer[i] != '/')
                throw badRequest();
            httpVersion += buffer[i];
            subState = DIGIT;
            break;
        case DIGIT:
            if (buffer[i] < '0' || buffer[i] > '9')
                throw badRequest();
            if (httpVersion[httpVersion.size() - 1] != '.')
                subState = DOT;
            else
                subState = CR_STATE;
            httpVersion += buffer[i];
            break;
        case DOT:
                if (buffer[i] != '.')
                throw badRequest();
            httpVersion += buffer[i];
            subState = DIGIT;
            break;
        case CR_STATE:
            if (buffer[i] != CR)
                throw badRequest();
            subState = LF_STATE;
            break;
        case LF_STATE:
            if (buffer[i] != LF)
                throw badRequest();
            mainState = HEADER;
            subState = FIELD_NAME;
            break;
        default:
            break;
    }
}

void Request::parseHeader(char *buffer, int i)
{
    switch (subState)
    {
        case FIELD_NAME:
            if (buffer[i] == CR && fieldName.empty())
            {
                subState = LF_STATE;
                break;
            }
            if (isWhiteSpace(buffer[i]))
                throw badRequest();
            if (buffer[i] == ':')
            {
                if (fieldName.empty())
                    throw badRequest();
                headers[fieldName] = "";
                subState = FIELD_VALUE;
            }
            else
                fieldName += buffer[i];
            break;
        //we will discuss about OWS
        // case OWS:
        //     if (!isWhiteSpace(buffer[i]))
        //         fieldValue += buffer[i];
        //     subState = FIELD_VALUE;
        //     break;
        case FIELD_VALUE:
            if (buffer[i] == CR)
            {
                subState = LF_STATE;
                headers[fieldName] = fieldValue;
            }
            else
                fieldValue += buffer[i];
            break;
        case CR_STATE:
            if (buffer[i] != CR)
                throw badRequest();
            subState = LF_STATE;
            break;
        case LF_STATE:
            if (buffer[i] != LF)
            {
                throw badRequest();
            }
            if(fieldName.empty() && fieldValue.empty())
            {
                // if()
                mainState = DONE;
                // mainState = BODY;
            }
            else
                subState = FIELD_NAME;
            fieldName = "";
            fieldValue = "";
            break;
        default:
            break;
    }
}
void Request::printRequestElement()
{
    std::cout << "{" << method << "}" << std::endl;
    std::cout << "{" << requestTarget << "}" << std::endl;
    std::cout << "{" << httpVersion << "}" << std::endl;

    std::cout << "{==========}" << std::endl;

    for (std::map<std::string , std::string>::iterator ite = headers.begin(); ite != headers.end(); ite++)
    {
        std::cout << "{" << ite->first << "}:{" << ite->second << "}" << std::endl;
    }
}

void Request::parseBody(char *buffer, int i)
{
    (void)i;
    (void)buffer;

    // if (headers.find("Content-Length") != headers.end())
    // {
        
    // }

    // switch(subState)
    // {
    //     case :
    // }
    
    std::cout << "DONE" << std::endl;
    mainState = DONE;

}

void Request::handle_request(char *buffer, int bytesRec)
{
    for(int i = 0; i < bytesRec; i++)
    {
        std::cout << "->" << i << "=>" << buffer[i] << std::endl;
        switch (mainState)
        {
            case REQUEST_LINE:
                parseRequestLine(buffer, i);
                break;
            case HEADER:
                parseHeader(buffer, i);
                break;
            case BODY:
                parseBody(buffer, i);
                break;
            default:
                break;
        }
    }
}

