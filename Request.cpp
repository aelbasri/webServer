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

std::string generateFilePath() {
    static int counter = 0;
    std::ostringstream oss;
    oss << UPLOAD_DIRECTORY << "file_" << counter++;
    return oss.str();
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

void Request::parseRequestLine(char *buffer, long i)
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
                queryValue += buffer[i];
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

void ft_trim(std::string &fieldValue)
{
    size_t i = 0;
    size_t len = fieldValue.size() - 1; 
    while (isWhiteSpace(fieldValue[i]))
    {
        if (i > 1)
            throw Request::badRequest();
        i++;
    }
    
    size_t start = i;
    i = len;
    while (isWhiteSpace(fieldValue[i]))
    {
        if (i < len)
            throw Request::badRequest();
        i--;
    }
    size_t end = i;
    fieldValue = fieldValue.substr(start, end - start + 1);
}

void Request::parseHeader(char *buffer, long i)
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
                subState = OWS;
            }
            else
                fieldName += buffer[i];
            break;
        //we will discuss about OWS
        case OWS:
            if (!isWhiteSpace(buffer[i]))
                fieldValue += buffer[i];
            subState = FIELD_VALUE;
            break;
        case FIELD_VALUE:
            if (buffer[i] == CR)
            {
                subState = LF_STATE;
                //trim
                ft_trim(fieldValue);
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
                mainState = WAIT;
                if (headers.find("Content-Length") != headers.end())
                {
                    subState = CONTLEN;
                    std::cout << "saad mrtah " << contentLength << "==" << maxBodySize << std::endl;
                    // exit(10);
                    contentLength = strtol(headers["Content-Length"].c_str(), NULL, 10);
                    if (contentLength > maxBodySize)
                        throw badRequest();
                }
                else if(headers["Transfer-Encoding"] == "chunked")
                    subState = CHUNK_HEADER;
                else
                    mainState = DONE;
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

void Request::parseBody(char *buffer, long &i, long bytesRec)
{
    // (void)i;
    // (void)bytesRec;
    // (void)buffer;
    int toBeConsumed = 0;
    switch (subState)
    {
        
        case CONTLEN :
            //check is open
            if (!contentFile.is_open())
            {
                if (_contentFile.empty())
                    _contentFile = "/tmp/.contentFile";

                _contentFile = generateFilePath();
                std::cout << "ghaanktbo f :" << _contentFile << std::endl;

                // exit (10); 
                contentFile.open(_contentFile.c_str(), std::ios::binary);
                if (!contentFile.is_open())
                    exit(22);
            }
            toBeConsumed = std::min(bytesRec - i, contentLength - consumed);      
            contentFile.write(buffer + i, toBeConsumed);
            // std::cout << "offset:" << i << "== content length:" << toBeConsumed << std::endl;
            i += toBeConsumed;
            consumed += toBeConsumed;
            // std::cout << "consumed:" << consumed << "== content length:" << contentLength << std::endl;
            if (consumed == contentLength)
            {
                mainState = DONE;
                consumed = 0;
                toBeConsumed = 0;
            }
            break;
        case CHUNK_HEADER:
            if (!contentFile.is_open())
            {
                if (_contentFile.empty())
                    _contentFile = "/tmp/.contentFile";

                _contentFile = generateFilePath();
                std::cout << "ghaanktbo fchunk:" << _contentFile << std::endl;

                // exit (10); 
                contentFile.open(_contentFile.c_str(), std::ios::binary);
                if (!contentFile.is_open())
                    exit(22);
            }
            if (buffer[i] == CR)
                subState = LF_STATE;
            else
                chunkSizeS += buffer[i];
            break;
        case LF_STATE:
            if (buffer[i] != LF)
            {
                throw badRequest();
            }
            else if (!chunkSizeS.empty())
            {
                std::stringstream ss;
                ss << std::hex << chunkSizeS;
                ss >> chunkSizeL;
                chunkSizeS = "";
            }
            else 
            {
                subState = CHUNK_HEADER;
                break;
            }
            if(chunkSizeL == 0)
                mainState = DONE;
            else
            {
                toBeConsumed = 0;
                consumed = 0;
                subState = LOAD_CHUNK;
            }
            break;
        case LOAD_CHUNK:
            toBeConsumed = std::min(bytesRec - i, chunkSizeL - consumed);      
            contentFile.write(buffer + i, toBeConsumed);
            i += toBeConsumed;
            consumed += toBeConsumed;
            contentBodySize += consumed;
            if (contentBodySize > maxBodySize)
                throw badRequest();
            if (consumed == chunkSizeL)
                subState = LF_STATE;
            break;
        default :
            break;
    }
    // std::cout << "DONE" << std::endl;
    // mainState = DONE;

}

void Request::closeContentFile()
{
    contentFile.close();
}

void Request::handle_request(char *buffer)
{
    for(; offset < bytesRec; offset++)
    {
        switch (mainState)
        {
            case REQUEST_LINE:
                parseRequestLine(buffer, offset);
                break;
            case HEADER:
                parseHeader(buffer, offset);
                break;
            case WAIT:
                std::cout << "WAAAAAAIT:" << "offset: " << offset << ", bytesRec:" << bytesRec << std::endl;
                goto exit_request_parsing;
                break;
            case BODY:
                parseBody(buffer, offset, bytesRec);
                break;
            default:
                break;
        }
    }
    exit_request_parsing :       
        return ;
}

