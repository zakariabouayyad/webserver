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
		size_t	filesize;
		size_t	totalSent;
		char*	buffer;
		void	initialize(request& request);
		void	sendHeader(int connection_socket, request& request);
		int		sendBody(int connection_socket, request& request);
};

#endif
