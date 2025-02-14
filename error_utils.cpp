#include "Response.hpp"

template <typename T>
std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

std::string getErrorPage(int status, server *serv) {
    std::map<std::string, std::string> errorPages = serv->GetErr();

    std::string statusStr = NumberToString(status);
    std::map<std::string, std::string>::const_iterator it = errorPages.find(statusStr);
    if (it != errorPages.end()) {
        return it->second;
    }
    return "";
}

std::string getCustomHtmlString(int status, std::string message)
{
    std::string customHtml = "<html><head><title>";
    customHtml += NumberToString(status);
    customHtml += " ";
    customHtml += message;
    customHtml += "</title></head><body><h1>";
    customHtml += NumberToString(status);
    customHtml += " ";
    customHtml += message;
    customHtml += "</h1></body></html>";
    return customHtml;
}

void setError(int status, std::string message, Response &response, server *serv) {
    std::string customHtml = "";
    std::string path = getErrorPage(status, serv);
    if (path.empty()) {
        std::cout << "Error page not found" << std::endl;
        // use custom html string
        customHtml = getCustomHtmlString(status, message);
    }
    else
    {
        // check if file exists
        std::ifstream file(path.c_str());
        if (!file.is_open()) {
            std::cout << "Error page not found" << std::endl;
            // use custom html string
            customHtml = getCustomHtmlString(status, message);
        }
        file.close();
    }

    std::string connection = "close";
    std::string contentType = getMimeType(path);

    response.setHttpVersion(HTTP_VERSION);
    response.setStatusCode(status);
    response.setReasonPhrase(message);
    if (customHtml.empty())
        response.setFile(path);
    else
        response.setTextBody(customHtml);

    response.setContentLength();
    response.addHeader(std::string("Content-Type"), contentType);
    response.addHeader(std::string("Connection"), connection);
}

