#include "Response.hpp"
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
