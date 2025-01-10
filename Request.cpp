#include "Request.hpp"

bool isWhiteSpace(char c)
{
    if ((c >= 9 && c <= 13) || c == 32)
        return (true);
    return (false);
}

int Request::parseRequestLine(std::stringstream &bufferStream)
{
    std::string firstLine;
    std::vector<std::string> elements;

    getline(bufferStream, firstLine);
    size_t len = firstLine.size();

    if(firstLine[len - 1] != '\r' || firstLine.empty())
    {
        //throw BadRequestExeption
        perror("Bad request");
        return (-1);   
    }
    int start = 0;
    size_t i = 0;
    for(; i < len - 1; i++)
    {
        if (isWhiteSpace(firstLine[start]))
        {
            //throw BadRequestExeption
            perror("Bad request");
            return (-1);   
        }
        if (isWhiteSpace(firstLine[i]))
        {
            elements.push_back(firstLine.substr(start, i - start));
            start = i + 1;
        }
    }
    elements.push_back(firstLine.substr(start, i - start));

    // //==
    // for(size_t j = 0; j < elements.size(); j++)
    // {
    //     std::cout << "--->" << '{' << elements[j] << '}' << std::endl;
    // }
    return (0);
}