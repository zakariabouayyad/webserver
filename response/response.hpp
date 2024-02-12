#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <sstream>
#include <fstream>
#include "../request/request.hpp"
class request;

class response{
	public:
		response();
		int		firstT;
		time_t resTime;
		std::string	header;
		size_t	filesize;
		size_t	totalSent;
		char*	buffer;
		void	initialize(request& request);
		int		sendHeader(int connection_socket, request& request);
		int		sendBody(int connection_socket);
		void	reset();
};

#endif
