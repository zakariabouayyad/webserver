#include "Cgi.hpp"
#include <sys/time.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


Cgi::Cgi(server *serv,  request *req): MyServer(serv),  MyRequest(req){
	this->env = NULL;
	this->arg = NULL;
	this->status = 0;
	init();
	set_arg();
	this->filename = "/tmp/file_all" + generateRandomFileName();
	this->filebody = "/tmp/file_body" + generateRandomFileName();
	this->filebodysend = "/tmp/file_sendbody" + generateRandomFileName();
}

Cgi::~Cgi()
{
	int i = -1;
	while(this->env[++i])
		delete this->env[i];
	delete [] this->env;
	i = -1;
	while (this->arg[++i])
		delete this->arg[i];
	delete [] this->arg;
}

char* strjoin(const std::string& s1, const std::string& s2) {
    size_t len = s1.length() + s2.length();
    char* result = new char[len + 1];
    strcpy(result, s1.c_str());
    strcat(result, s2.c_str());
    return result;
}


std::string toUpperCase(const std::string& input) {
    std::string result = input;
    
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::toupper(result[i]);
    }
    return result;
}

void replace_(std::string &input) {
    size_t pos = input.find('-');
    while (pos != std::string::npos) {
        input.replace(pos, 1, "_");
        pos = input.find('-', pos + 1);
    }
}


std::string getroot(const std::string& filepath) {
    size_t lastSeparator = filepath.find_last_of("/\\");
    if (lastSeparator != std::string::npos)
        return filepath.substr(0, lastSeparator);
	else
        return filepath;
}

void	Cgi::init(){
	std::map<string, string> mapenv;
	std::map<string, string> headerenv = MyRequest->headerFields;
	std::map<string, string>::iterator it = headerenv.begin();
	for(; it != headerenv.end(); it++){
		string key = toUpperCase(it->first);
		replace_(key);
		string value = it->second;
		mapenv[key] = value;
	}
	mapenv["SERVER_PROTOCOL"] = "HTTP/1.1";
	mapenv["GATEWAY_INTERFACE"] = "CGI/1.1";
	mapenv["SERVER_SOFTWARE"] = "webserv/1.0";
	mapenv["REDIRECT_STATUS" ] = "0";
	mapenv["SERVER_NAME"] = this->MyServer->getServerName();
	mapenv["SERVER_PORT"] = this->MyServer->getPort();
	mapenv["PATH_INFO"] = this->MyRequest->getFilePath();
	mapenv["PATH_TRANSLATED"] = this->MyRequest->getFilePath();
	mapenv["SCRIPT_NAME"] = this->MyRequest->getFilePath();
	mapenv["DOCUMENT_ROOT"] = getroot(this->MyRequest->getFilePath());
	mapenv["HTTP_HOST"] = this->MyServer->get_ip();
	mapenv["HTTP_CONNECTION"] = this->MyRequest->headerFields["Connection"];
	mapenv["HTTP_ACCEPT"] = this->MyRequest->headerFields["Accept"];
	mapenv["HTTP_USER-AGENT"] = this->MyRequest->headerFields["User-Agent"];
	mapenv["HTTP_COOKIE"] = this->MyRequest->headerFields["Cookie"];
	mapenv["REQUEST_URI"] = this->MyRequest->getFilePath();;
	if (this->MyRequest->getMethod() == "POST"){
		mapenv["CONTENT_TYPE"] = this->MyRequest->headerFields["Content-Type"];
		mapenv["CONTENT_LENGTH"] = this->MyRequest->headerFields["Content-Length"];
		mapenv["REQUEST_METHOD"] = this->MyRequest->getMethod();
	}
	else if (this->MyRequest->getMethod() == "GET"){
		mapenv["QUERY_STRING"] = this->MyRequest->getQueryString();
		mapenv["CONTENT_TYPE"] = this->MyRequest->headerFields["Content-Type"];
		mapenv["CONTENT_LENGTH"] = "";
		mapenv["REQUEST_METHOD"] = this->MyRequest->getMethod();
	}
	std::map<string, string>::iterator it2 = mapenv.begin();
	for(; it2 != mapenv.end(); it2++){
		string key = toUpperCase(it2->first);
		string value = it2->second;
	}
	size_t len = mapenv.size();
	this->env = new char*[len+1];
	size_t i = 0;
	 for (std::map<std::string, std::string>::const_iterator it = mapenv.begin(); it != mapenv.end(); ++it) {
        this->env[i] = strjoin(it->first + "=", it->second);
        ++i;
    }
	
	this->env[len] = NULL;
}

void	Cgi::set_arg(){
	this->arg = new char*[3];
	arg[0] = strdup(MyRequest->cgi_exe.first.c_str());
	arg[1] = strdup(this->MyRequest->getFilePath().c_str());
	arg[2] = NULL;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos) {
        return "";  // String is all whitespaces
    }
    return str.substr(start, end - start + 1);
}

std::string Cgi::generateRandomFileName() {
	std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int length = 8;  // Adjust the length as needed

	std::string randomFileName;
	srand(static_cast<unsigned>(time(0)));

	for (int i = 0; i < length; ++i) {
		int randomIndex = rand() % characters.length();
		randomFileName += characters[randomIndex];
	}

	return randomFileName;
}

void	Cgi::parseHeader(std::vector<std::string> header, size_t len){
	std::stringstream ss;
	ss << len;
	string head;
	string	status;
	string	type;
	string	location;
	if (!header.empty()){
		 for (size_t i = 0; i < header.size(); ++i) {

			if(header[i].find("Status:") != std::string::npos){
				status = "HTTP/1.1 " + trim(header[i].substr(7)) + "\r\n";
			}
			else if(header[i].find("Content-type:") != std::string::npos){
				type = "Content-type: text/html; charset=UTF-8\r\n";
			}
			else if(header[i].find("Location:") != std::string::npos){
				location = header[i] + "\r\n";
			}
			else
				head +=  header[i] + "\r\n";
		}
	}
	head += "Content-Length: " + ss.str() + "\r\n";
	head = location + head;
	if (status.empty())
		head = "HTTP/1.1 200 OK\r\n" + head;
	else
		head = status + head;
	this->header = head + "\r\n";
}

std::vector<std::string> splitString(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);

    while (!iss.eof()) {
        std::string line;
        std::getline(iss, line, '\n');
		line = trim(line);
        if (!line.empty()) {
            result.push_back(line);
        }
    }
    return result;
}

void sighandle(int sig)
{
	(void)sig;
	exit(502);
}


int Cgi::ParseAll(){
	std::string body;
	std::ifstream file(this->filename);
    if (!file.is_open()) 
        return 502;
    std::ostringstream content;
    content << file.rdbuf();
    file.close();
	body = content.str();

	std::vector<std::string> result;
	std::string header;
	bool	is_header = 0;
	if (body.find("\r\n\r\n") != std::string::npos){
		is_header = 1;
		header = body.substr(0,body.find("\r\n\r\n") + 4);
		body = body.substr(body.find("\r\n\r\n") + 4);
		result = splitString(header);
	}
	body = trim(body);

	parseHeader(result, body.size());

	std::ofstream BodysendFile(filebodysend, std::ios::out | std::ios::trunc);
	if (!BodysendFile.is_open())
		return 502;
	BodysendFile << body;
	BodysendFile.close();
	this->body = filebodysend;
	return 1;
}

void Cgi::exe(){
	std::ofstream bodyFile(filebody, std::ios::out | std::ios::trunc);
	if (!bodyFile.is_open())
		this->status = 502;
	bodyFile << MyRequest->theBody;
	bodyFile.close();
	pid = fork();
	if (pid == -1){
		this->status = 500;
		return ;
	}
	if (!pid) {
		int	fd = open(filename.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666);
		if (fd == -1)
			exit(502);
		if (dup2(fd, 1) == -1) {
			close(fd);
			exit(502);
		}
		close(fd);
		if (MyRequest->getMethod() == "POST"){
			int fd1 = open(filebody.c_str(), O_RDONLY);
			if (fd1 == -1)
				exit(502);
			if (dup2(fd1, 0) == -1){
				close(fd1);
				exit(502);
			}
			close(fd1);
		}
		signal(SIGALRM, sighandle);
		alarm(5);
		execve(this->arg[0], this->arg, this->env);
		exit(502);
	}
}


int	Cgi::waitcheck(){
	if (this->status == 502 || this->status == 504 || this->status == 500)
		return this->status;
	int sts;
    int	it = waitpid(pid, &sts, WNOHANG);
	if (it == -1)
		return 500;
	if (it > 0){
		if ( WIFEXITED(sts) && WEXITSTATUS(sts) != 0 )
			return 502;
		else if ( WIFSIGNALED(sts) && WTERMSIG(sts) == SIGALRM )
			return 504;
		else
			return 1;
	}
	return 0;
}