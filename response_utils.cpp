#include "Response.hpp"
#include "Connection.hpp"
#include "configfile/location.hpp"

std::map<std::string, std::string> initializeMimeTypes() {
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html; charset=UTF-8";
    mimeTypes[".htm"] = "text/html; charset=UTF-8";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/vnd.microsoft.icon";
    mimeTypes[".webp"] = "image/webp";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".mpeg"] = "video/mpeg";
    mimeTypes[".wav"] = "audio/wav";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".ogg"] = "audio/ogg";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".tar"] = "application/x-tar";
    mimeTypes[".gz"] = "application/gzip";
    mimeTypes[".csv"] = "text/csv";
    return mimeTypes;
}

std::string getMimeType(const std::string& filename) {
    static const std::map<std::string, std::string> mimeTypes = initializeMimeTypes();

    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos);
        std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

bool methodAllowed(const std::string& method, const std::vector<std::string>& allowedMethods) {
    for (size_t i = 0; i < allowedMethods.size(); i++) {
        if (method == allowedMethods[i]) {
            return true;
        }
    }
    return false;
}

location* getLocationMatch(std::string target, location *locations, int size) {
    location *best_match = nullptr;
    size_t best_match_length = 0;

    // std::cout << "size: " << size << std::endl;

    for (int i = 0; i < size; i++) {
        std::string locType = locations[i].GetType_of_location();
        // std::cout << "Lets compare: " << locType << " with " << target << std::endl;
        if (locType == target) {
            return &locations[i];
        }
        if (target.find(locType) == 0 && locType.length() > best_match_length) {
            best_match = &locations[i];
            best_match_length = locType.length();
        }
    }
    // std::cout << std::endl;

    if (best_match == nullptr)
    {
        // std::cout << "walo abro" << std::endl;
        for (int i = 0; i < size; i++) {
            if (locations[i].GetType_of_location() == "/") {
                best_match = &locations[i];
                break;
            }
        }
    }
    return (best_match);
}


// --  HADCHI DYAL DEEPSEEK WLAH MADYALI -- //

// Helper function to trim whitespace from a string
std::string _trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Function to parse CGI output and handle edge cases
int parseCGI(std::string &CgiOutput, Response &response) {
    // Check if the output is empty
    if (CgiOutput.empty()) {
        std::cerr << "Error: CGI output is empty." << std::endl;
        return 1;
    }

    // Find the end of headers (double CRLF)
    size_t pos = CgiOutput.find("\r\n\r\n");
    if (pos == std::string::npos) {
        std::cerr << "Error: Malformed CGI output (missing header/body separator)." << std::endl;
        return 1;
    }

    // Split into headers and body
    std::string headers = CgiOutput.substr(0, pos);
    std::string body = CgiOutput.substr(pos + 4);

    // Split headers into lines
    std::vector<std::string> headerLines = StringToLines(headers);
    if (headerLines.empty()) {
        std::cerr << "Error: No headers found in CGI output." << std::endl;
        return 1;
    }

    // Parse each header line
    for (size_t i = 0; i < headerLines.size(); i++) {
        size_t found_at = headerLines[i].find(':');
        if (found_at == std::string::npos) {
            std::cerr << "Warning: Malformed header line: " << headerLines[i] << std::endl;
            continue; // Skip malformed headers
        }

        std::string key = _trim(headerLines[i].substr(0, found_at));
        std::string value = _trim(headerLines[i].substr(found_at + 1));

        if (key.empty() || value.empty()) {
            std::cerr << "Warning: Empty key or value in header: " << headerLines[i] << std::endl;
            continue; // Skip empty keys or values
        }

        // Handle the "Status" header separately
        if (key == "Status") {
            int status = std::atoi(value.c_str());
            response.setStatusCode(status);
            std::string reasonPhrase = value.substr(4); // Skip the status
            response.setReasonPhrase(reasonPhrase);
        } else {
            response.addHeader(key, value);
        }
    }

    // Set the response body
    if (body.empty()) {
        std::cerr << "Warning: CGI output body is empty." << std::endl;
    }
    response.setTextBody(body);

    return 0;
}

FileState getFileState(const char *path) {
    struct stat fileInfo;
    if (stat(path, &fileInfo) == 0) {
        if (S_ISREG(fileInfo.st_mode)) {
            return FILE_IS_REGULAR;
        } else if (S_ISDIR(fileInfo.st_mode)) {
            return FILE_IS_DIRECTORY;
        } else {
            return FILE_IS_OTHER;
        }
    } else {
        return FILE_DOES_NOT_EXIST;
    }
}

// deepseek a3chiri 
std::string listDirectoryHTML(const char *path) {
    std::ostringstream htmlOutput;

    DIR *dir = opendir(path);
    if (!dir) {
        htmlOutput << "Error: Could not open directory " << path << std::endl;
        return htmlOutput.str();
    }

    htmlOutput << "<html>\n"
               << "<head><title>Index of " << path << "</title></head>\n"
               << "<body>\n"
               << "<h1>Index of " << path << "</h1><hr><pre>\n"
               << "<a href=\"../\">../</a>\n";

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path
        std::string fullPath = std::string(path) + "/" + entry->d_name;

        // Get file info using lstat
        struct stat fileInfo;
        if (lstat(fullPath.c_str(), &fileInfo) == 0) {
            // File name (linked)
            htmlOutput << "<a href=\"" << entry->d_name << "\">" << entry->d_name << "</a>";

            // Format the output to align columns
            htmlOutput << std::setw(50 - strlen(entry->d_name)) << " "; // Adjust spacing

            // Modification date
            char dateBuffer[20];
            strftime(dateBuffer, sizeof(dateBuffer), "%d-%b-%Y %H:%M", localtime(&fileInfo.st_mtime));
            htmlOutput << dateBuffer << "  ";

            // File size
            htmlOutput << std::setw(10) << std::right << fileInfo.st_size;

            // Handle symbolic links
            if (S_ISLNK(fileInfo.st_mode)) {
                char target[1024];
                ssize_t len = readlink(fullPath.c_str(), target, sizeof(target) - 1);
                if (len != -1) {
                    target[len] = '\0';
                    htmlOutput << " -&gt; " << target;
                }
            }

            htmlOutput << "\n";
        } else {
            htmlOutput << "Error: Could not stat " << fullPath << "\n";
        }
    }

    htmlOutput << "</pre><hr></body>\n</html>\n";

    closedir(dir);

    return htmlOutput.str();
}

template <typename T>
std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

std::string getCustomHttpResponsePage(int status, server *serv) {
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

void setHttpResponse(int status, std::string message, Response &response, server *serv) {
    std::string customHtml = "";
    std::string path = getCustomHttpResponsePage(status, serv);
    if (path.empty()) {
        std::cout << "Error page not found" << std::endl;
        // use custom html string
        customHtml = getCustomHtmlString(status, message);
    }
    else
    {
        // check if file exists
        std::ifstream file(path.c_str());
        if (!file.good()) {
            std::cout << "Error page not found" << std::endl;
            // use custom html string
            customHtml = getCustomHtmlString(status, message);
        }
        file.close();
    }

    std::string connection = "close";
    std::string contentType = getMimeType("foo.html");

    response.setHttpVersion(HTTP_VERSION);
    response.setStatusCode(status);
    response.setReasonPhrase(message);
    response.addHeader(std::string("Content-Type"), contentType);
    response.addHeader(std::string("Connection"), connection);
    if (customHtml.empty())
    {
        int length = response.setFileBody(path);
        response.setContentLength(length);
    }
    else
    {
        response.setTextBody(customHtml);
        response.setContentLength(customHtml.size());
    }
}

std::string getFilenameFromPath(std::string path) {
    (void)path;
    return (std::string("someRandomName"));
}





