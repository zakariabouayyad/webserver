#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <vector>
#include <map>
#include "../server.hpp"
#include "../request/request.hpp"

class	server;
class	request;
class Cgi
{
private:
	char		**env;
	server		*MyServer;
	request		*MyRequest;
	char		**arg;
	std::string filename;
	std::string filebody;
	std::string filebodysend;
	pid_t		pid;
	void	parseHeader(std::vector<std::string> header, size_t len);
	std::string generateRandomFileName();
public:
	Cgi(server *serv, request *req);
	// Cgi(const Cgi& other);
    // Cgi& operator=(const Cgi& other);
	~Cgi();
	// Cgi();
	std::string	header;
	std::string	body;
	int		status;
	void	init();
	void	exe();
	void	set_arg();
	int ParseAll();
	int	waitcheck();
};

#endif