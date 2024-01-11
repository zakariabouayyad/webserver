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
	string	res;
	public:
		int	set_res(int connection_socket, request&);
		string	get_res();
};

#endif
