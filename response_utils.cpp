#include "Response.hpp"
#include "Connection.hpp"
#include "configfile/location.hpp"
#include "log.hpp"

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

std::string set_cookie(const std::string& name, const std::string& value) {
    std::string cookie =  name + "=" + value + "; Max-Age=3600" + "; Path=/\r\n";
    return cookie;
}


void parseCredentials(const std::string& input, std::string& username, std::string& password, bool& rememberMe) {
    size_t userPos = input.find("username=");
    if (userPos != std::string::npos) {
        size_t userStart = userPos + 9;
        size_t userEnd = input.find('&', userStart);
        username = input.substr(userStart, userEnd - userStart);
    }

    size_t passPos = input.find("password=");
    if (passPos != std::string::npos) {
        size_t passStart = passPos + 9;
        size_t passEnd = input.find('&', passStart);
        password = input.substr(passStart, passEnd - passStart);
    }

    size_t rememberPos = input.find("remember_me=");
    if (rememberPos != std::string::npos) {
        size_t rememberStart = rememberPos + 12;
        size_t rememberEnd = input.find('&', rememberStart);
        std::string rememberValue = input.substr(rememberStart, rememberEnd - rememberStart);
        rememberMe = (rememberValue == "on");
    } else {
        rememberMe = false;
    }
}
bool isRememberMeOn(const std::string& input) {
    size_t rememberPos = input.find("remember_me=");
    if (rememberPos == std::string::npos) {
        return false; 
    }

    size_t valueStart = rememberPos + 12;
    size_t valueEnd = input.find('&', valueStart);

    std::string rememberValue;
    if (valueEnd == std::string::npos) {
        rememberValue = input.substr(valueStart);
    } else {
        rememberValue = input.substr(valueStart, valueEnd - valueStart);
    }
    return (rememberValue == "on");
}

std::string generateSecureToken(size_t length = 32) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t charsetSize = sizeof(charset) - 1; 

    std::string token;
    token.reserve(length);

    std::srand(static_cast<unsigned int>(std::time(0)));

    for (size_t i = 0; i < length; ++i) {
        token += charset[std::rand() % charsetSize];
    }
    return token;
}


bool isTokenExist(const std::vector<std::pair<std::string, std::string> >& userTokens, const std::string& token) {
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = userTokens.begin(); it != userTokens.end(); ++it) {
        if (it->second == token) { 
            return true; 
        }
    }
    return false; 
}

void handleCGI(server *server, Response &response, Request &request)
{

    std::string username, password;
    bool remember_me;
    std::string s;
    CGI _cgi("./cgi-bin/login.py");

    // response.
    std::string _userToken = "";
    if (isTokenExist(server->GetUserToken(), _userToken))
    {
        // std::
        // TODO: send username related to the ID session "PAGE HTML"
        return ;
    }

    std::ifstream myfile("/tmp/.contentData");
    if (!myfile.is_open()) {
        webServLog("Failed to open /tmp/.contentData", ERROR);
        response.setStatusCode(500);
        return;
    }
    std::string line;
    while (getline(myfile, line)) {
        s += line + "\n";
    }
    myfile.close();
    std::string executable = _cgi.RunCgi(s);
    std::string postData;
    if (request.getMethod() == "POST") {
        postData = s;
    }

    std::string cgiOutput = _cgi.RunCgi(postData);
    std::cout << "the cgi autput is {" << cgiOutput << "}" <<  std::endl;
    parseCredentials(s, username, password, remember_me);

    //create new session
    if (remember_me) {
        std::string sessionToken = generateSecureToken();
        server->SetUserToken(make_pair(username, sessionToken));
        std::string cookie = set_cookie("session_token", sessionToken);
        response.addHeader("Set-Cookie", cookie);
    }
    std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [CGI executed]";
    webServLog(logMessage, INFO);
    return;
}

std::map<std::string, std::string> extractHeaders(const std::string& output) {
    std::cout << "======================================" << std::endl;
    std::map<std::string, std::string> headers;
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            headers[key] = value;
            std::cout << "HEADER: " << key << " = " << value << std::endl;
        }
    }
    std::cout << "======================================" << std::endl;
    return headers;
}

std::string extractBody(const std::string& output) {
    // size_t pos = output.find("\r\n\r\n");
    size_t pos = output.find("\n\n");
    if (pos != std::string::npos) {
        return output.substr(pos + 2);
    }
    return "";
}

std::string convertQueryMapToString(const std::map<std::string, std::string>& query) {
    std::ostringstream stream;
    for (std::map<std::string, std::string>::const_iterator it = query.begin(); it != query.end(); ++it) {
        stream << it->first << "=" << it->second << "&";
    }
    std::string queryString = stream.str();
    if (!queryString.empty()) {
        queryString.pop_back(); // Remove the trailing '&'
    }
    return queryString;
}

void handleCGI2(server *serv, Response &response, Request &request) {
    (void)serv;

    int *stdin_pipe = response.getCGIPIPE();
    if (stdin_pipe[0] == -1 && stdin_pipe[1] == -1)
    {
        if (pipe(stdin_pipe) == -1) {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to create pipe]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
    }

    if (request.getMethod() == "POST") {
        if (request.getState() == WAIT)
        {
            request.setWriteInPipe(true);
            request.setFd(stdin_pipe[1]);
            request.setState(BODY);
            response.setProgress(POST_HOLD);
            // process remaining body in buffer
            request.handle_request(request.getBuffer());
            return ;
        }
    }

    // Get the path to the CGI script from the request
    std::string scriptPath = ".";
    scriptPath += request.getRequestTarget();

    // Set up environment variables for the CGI script
    std::map<std::string, std::string> env;
    env["REQUEST_METHOD"] = request.getMethod();
    env["QUERY_STRING"] = convertQueryMapToString(request.getQuery());
    env["CONTENT_TYPE"] = request.getHeader("Content-Type");
    env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
    env["HTTP_COOKIE"] = request.getHeader("Cookie");

    // Add other headers as environment variables
    for (const auto& header : request.getHeaders()) {
        std::string envVar = "HTTP_" + header.first;
        std::replace(envVar.begin(), envVar.end(), '-', '_');
        std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
        env[envVar] = header.second;
        // std::cout << "ENV: " << envVar << " = " << header.second << std::endl;
    }

    int stdout_pipe[2];
    if (pipe(stdout_pipe) == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to create pipe]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }

    // Fork to execute the CGI script
    pid_t pid = fork();
    if (pid == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);

        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Failed to fork]";
        webServLog(logMessage, ERROR);
        throw server::InternalServerError();
    }

    if (pid == 0) { // Child process (CGI script)
        // std::cout << "CHILD PROCESS" << std::endl;
        // std::cout << "CHILD READING FROM: " << stdin_pipe[0] << std::endl;
        // std::cout << "CHILD WRITING TO: " << stdout_pipe[1] << std::endl;

        // Close the unused ends of the pipes
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        // Redirect stdin from the pipe
        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        // Redirect stdout to the pipe
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[1]);

        // Set environment variables
        for (const auto& var : env) {
            setenv(var.first.c_str(), var.second.c_str(), 1);
        }

        // Execute the CGI script
        if (scriptPath.find(".py") != std::string::npos) {
            execl("/usr/bin/python3", "python3", scriptPath.c_str(), (char *)0);
        } else if (scriptPath.find(".php") != std::string::npos) {
            execl("/usr/bin/php", "php", scriptPath.c_str(), (char *)0);
        } else {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [Unsupported script type]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
    } else { // Parent process (server)
        // std::cout << "PARENT PROCESS" << std::endl;
        // std::cout << "PARENT READING FROM: " << stdout_pipe[0] << std::endl;

        // close the unused ends of the pipes
        close(stdin_pipe[0]);
        close(stdin_pipe[1]); // body already written to pipe
        close(stdout_pipe[1]);

        // Read the output from the CGI script
        std::ostringstream output;
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
            // std::cout << "Bytes read: " << bytesRead << std::endl;
            // std::cout << "BUFFER: " << buffer << std::endl;
            output.write(buffer, bytesRead);
        }
        close(stdout_pipe[0]);

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        // std::cout << "SCRIPT OUTPUT: (" << output.str() << ")" << std::endl;

        // Check if the script executed successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            // extract headers and body from the output
            std::map<std::string, std::string> headers = extractHeaders(output.str());
            std::string responseBody = extractBody(output.str());
            for (const auto& header : headers) {
                response.addHeader(header.first, header.second);
            }
            response.setHttpVersion("HTTP/1.1");
            response.setStatusCode(200);
            response.setReasonPhrase("OK");
            response.setTextBody(responseBody);
            int length = responseBody.size();
            response.setContentLength(length);
            response.setProgress(BUILD_RESPONSE);
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [200] [OK] [CGI executed]";
            webServLog(logMessage, INFO);
        } else {
            std::string logMessage = "[" + request.getMethod() + "] [" + request.getRequestTarget() + "] [500] [Internal Server Error] [CGI script failed]";
            webServLog(logMessage, ERROR);
            throw server::InternalServerError();
        }
    }
}
