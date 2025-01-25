#include "Response.hpp"

std::string getMimeType(const std::string& filename) {
    static const std::map<std::string, std::string> mimeTypes = {
        {".html", "text/html"},
        {".htm", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".txt", "text/plain"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".ico", "image/vnd.microsoft.icon"},
        {".webp", "image/webp"},
        {".mp4", "video/mp4"},
        {".mpeg", "video/mpeg"},
        {".wav", "audio/wav"},
        {".mp3", "audio/mpeg"},
        {".ogg", "audio/ogg"},
        {".pdf", "application/pdf"},
        {".zip", "application/zip"},
        {".tar", "application/x-tar"},
        {".gz", "application/gzip"},
        {".csv", "text/csv"}
    };

    size_t dotPos = filename.rfind('.');
    if (dotPos != std::string::npos) {
        std::string extension = filename.substr(dotPos);
        if (mimeTypes.find(extension) != mimeTypes.end())
            return mimeTypes[extension];
    }
    return "application/octet-stream";
}
