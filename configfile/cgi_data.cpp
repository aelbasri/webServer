

#include "cgi_data.hpp"

CGI::CGI()
{
}

CGI::CGI(std::string __path, std::string __type)
{
	_path = __path;
	_type = __type;
}

CGI::~CGI()
{
}

void CGI::SetPath(std::string __path)
{
	_path = __path;
}

void CGI::SetType(std::string __type)
{
	_type = __type;
}

std::string CGI::GetPath()
{
	return (_path);
}

std::string CGI::GetType()
{
	return (_type);
}

std::string generate_unique_path()
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	std::stringstream ss;
	ss << "/tmp/." << tv.tv_sec << tv.tv_usec << ".html";
	return (ss.str());
}

std::string CGI::RunCgi(const std::string &requestBody)
{
    const char *command = "/usr/bin/python3 ./cgi-bin/login.py";
    FILE *pipe;
    char buffer[128];

    std::cout << "---------------->>>>>>>> " << requestBody << std::endl;

    pipe = popen(command, "w");
    if (!pipe)
    {
        std::cerr << "Error executing command." << std::endl;
        return "";
    }

    fwrite(requestBody.c_str(), 1, requestBody.size(), pipe);
    pclose(pipe);

    pipe = popen(command, "r");
    if (!pipe)
    {
        std::cerr << "Error executing command." << std::endl;
        return "";
    }

    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }

    pclose(pipe);
    return result;
}
