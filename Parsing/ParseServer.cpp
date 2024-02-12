
#include "../server.hpp"
using std::endl;
using std::cout;
std::vector<std::string> server::splitString(const std::string& input, const std::string& delm) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string     token;

    while (start < input.length()) {
        // Find the first non-whitespace character
        std::string::size_type nonWhitespace = input.find_first_not_of(delm.c_str(), start);

        // Find the next whitespace character
        std::string::size_type end = input.find_first_of(delm.c_str(), nonWhitespace);

        // Extract the substring between nonWhitespace and end
        if (nonWhitespace != std::string::npos) {
            if (end == std::string::npos)
                token = input.substr(nonWhitespace, end);
            else
                token = input.substr(nonWhitespace, end - nonWhitespace);
            tokens.push_back(token);
        }

        // Move to the next character after the found whitespace
        if (end == std::string::npos)
            start = end;
        else
            start = end + 1;
    }

    return tokens;
}


std::string server::throwmessage(int number, const std::string& str) {
    std::stringstream ss;
    ss << number;
    return  "config:line:" + ss.str() + ": " + str;
}

std::string server::withoutsemicolon(std::string str){
	if (!str.empty()) {
        size_t lastNonSemicolon = str.find_last_not_of(';');
        if (lastNonSemicolon != std::string::npos && lastNonSemicolon < str.length() - 1) {
            return str.substr(0, lastNonSemicolon + 1);
        }
    }

    return str;
}



bool server::isInMyList(const std::string& target, int &token) {
	std::vector<std::string> lst;
    lst.push_back("server_name");
    lst.push_back("listen");
    lst.push_back("root");
    lst.push_back("allow_methods");
    lst.push_back("autoindex");
    lst.push_back("index");
    lst.push_back("client_body_limit");
    lst.push_back("error_page");
	lst.push_back("upload");
	lst.push_back("cgi_exe");
	for (size_t i = 0; i < lst.size(); i++){
		if (lst[i].compare(target) == 0){
			token = i + 1;
			break;
		}
	}
	return std::find(lst.begin(), lst.end(), target) != lst.end();
}

bool server::isWhitespace(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isspace(static_cast<unsigned char>(*it))) {
            return false; // Found a non-whitespace character
        }
    }
    return true; // Only whitespace characters found
}


void	server::Myhostport(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "invalid input hostname:port."));

	// Find the position of ':'
    size_t colonPos = list[1].find(':');

    // Check if ':' exists in the string
    if (colonPos == std::string::npos)
        throw std::invalid_argument(throwmessage(line, "Error: Missing colon in input string."));

	// Extract hostname and port substrings
    setIp(list[1].substr(0, colonPos));
    std::string portStr = withoutsemicolon(list[1].substr(colonPos + 1));

    // Convert port string to integer
    char* endPtr;
    long parsedPort = std::strtol(portStr.c_str(), &endPtr, 10);

    // Check for conversion errors
    if (*endPtr != '\0' || parsedPort <= 0 || parsedPort > 65535)
        throw std::invalid_argument(throwmessage(line, "Error: Invalid port number."));

	setPort(portStr);
}


void	server::set_value(std::vector<std::string> list, int token, int line){
	
	switch (token)
	{
	case 1:
		Myserver_name(list, line);
		break;
	case 2:
		Myhostport(list, line);
		break;
	case 3:
		Myroot(list, line);
		break;
	case 4:
		Myallow_methods(list, line);
		break;
	case 5:
		Myautoindex(list, line);
		break;
	case 6:
		Myindex(list, line);
		break;
	case 7:
		Myclient_body_limit(list, line);
		break;
	case 8:
		Myerror_page(list, line);
		break;
	case 9:
		Myupload(list, line);
		break;
	case 10:
		Mycgi(list, line);
		break;
	default:
		break;
	}
}

void	server::seter(std::string str, int line){
	std::vector<std::string> list;
	int	token;

	list = splitString(str, " \t");
	if (list.empty() || list[0][0] == '#')
		return ;
	if (!isInMyList(list[0], token))
		throw std::invalid_argument(throwmessage(line, "Error: invalid input."));
	if (list[list.size() - 1][list[list.size() - 1].length() - 1] != ';')
		throw std::invalid_argument(throwmessage(line, "Error: the line have to end with ';'."));
	if (isWhitespace(str) && str.size() > 0)
		throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces."));
	int f = 0;
	for (size_t i = str.size() - 1; i > 0; i--){
		if (str[i] == ';')
			f++;
		if (f == 2)
			throw std::invalid_argument(throwmessage(line, "Error: More than one semicolon in the string."));
	}
	if (str[0] != '\t' || std::isspace(str[1]))
		throw std::invalid_argument(throwmessage(line, "Error: execept one TAB at the start."));
	if (std::isspace(str[str.length() - 1]))
		throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the end."));
	for (size_t i = 1; i < str.length() - 1; ++i) {
        if (std::isspace(str[i]) && std::isspace(str[i + 1]))
			throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces."));
	}
	set_value(list, token, line);
}

void	server::checkfirstline(std::string str, int line){
	if (std::isspace(str[0]) )
        throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the beginning."));
	if (std::isspace(str[str.length() - 1]))
		throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the end."));
	// Check for excessive whitespace between words
    for (size_t i = 0; i < str.length() - 1; ++i) {
        if (std::isspace(str[i]) && std::isspace(str[i + 1]))
			throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces."));
    }
	if (str.compare("server {"))
		throw std::invalid_argument(throwmessage(line, "Error: Invalid input."));
	this->line = line;
}

void	server::checklastline(std::string str, int line, int firstline){
	if (str.empty())
		throw std::invalid_argument(throwmessage(firstline, "Error: Invalid Input, must close your server with bracket '}' ."));
	if (std::isspace(str[0]) )
        throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the beginning."));
	if (std::isspace(str[str.length() - 1]))
		throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the end."));
	// Check for excessive whitespace between words
    for (size_t i = 0; i < str.length() - 1; ++i) {
        if (std::isspace(str[i]) && std::isspace(str[i + 1]))
			throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces."));
    }
	if (str.compare("}"))
		throw std::invalid_argument(throwmessage(line, "Error: Invalid Input"));
}	


void	server::setmylocation(std::map<int, std::string>::const_iterator &it, std::map<int, std::string> &server){
	std::map<int, std::string> locations;
	std::map<int, std::string>::const_iterator itf = it;
	int itfirst = it->first;
	for(; it != server.end() && it->second.find(")") == std::string::npos; ++it){
		if (it != itf && it->second.find("(") != std::string::npos)
			throw std::invalid_argument(throwmessage(itfirst, "Error: You have to close your location with ')'."));
		locations[it->first] = it->second;
	}
	if (it->second.find(")") != std::string::npos)
		locations[it->first] = it->second;
	else if (it == server.end())
		throw std::invalid_argument(throwmessage(itfirst, "Error: You have to close your location with ')'."));
	Location	local(locations);
	setLocations(local);
}

void	server::init(){
	setAutoindex(0);
	setClientBodyLimit(50);
	set_isdefault(1);
	set_my_default(-1);
	setRoot("public");
	setIndex("");
	setErrorPage(403, "errorPages/err403.html");
	setIp("localhost");
	setPort("8080");
	setServerName("wal7amaq");
	setAllowMethods("GET");
	setAllowMethods("POST");
	setAllowMethods("DELETE");
	setUpload(1);
}

bool comparePath(const Location& loc1, const Location& loc2)
{
    return loc1.getLocationName() > loc2.getLocationName();
}

void	server::parse(std::map<int, std::string> &server){
	init();
	std::map<int, std::string>::const_iterator it = server.begin();
	std::map<int, std::string>::reverse_iterator rit = server.rbegin();

	checkfirstline(it->second, it->first);
	checklastline(rit->second, rit->first, it->first);
	it++;
	for (; it != server.end(); it++)
    {
		if (it->second.find("}") == std::string::npos){
			if (it->second.find("(") != std::string::npos 
				&& it->second.find("#") == std::string::npos){
				setmylocation(it, server);
			}
			else
       			seter(it->second, it->first);
		}
	}

	// add in / location
	std::vector<Location> &local = this->locations;
	size_t i = 0;
	for (; i < local.size(); i++){
		if(local[i].getLocationName() == "/"){
			if (!local[i].a)
				local[i].setAutoindex(getAutoindex());
			if (!local[i].i)
				local[i].setIndex(getIndex());
			if (!local[i].r)
				local[i].setRoot(getRoot());
			if (!local[i].am)
				local[i].setVecAllowMethods(getAllowMethods());
			if (!local[i].u)
				local[i].setUpload(getUpload());
			if (!local[i].cg)
				local[i].setCgiExe(getCgiExe());
			break;
		}
	}
	if (i == local.size()){
		Location	loc(getRoot(), getIndex(), getAutoindex(), getAllowMethods());
		setLocations(loc);
	}
	std::sort(this->locations.begin(), this->locations.end(), comparePath);
	doublelocationcheck();
}

void	server::doublelocationcheck(){
	vector<Location> loc = this->locations;
	std::string locationName;
	for (size_t i = 0; i < loc.size(); i++)
	{
		locationName = loc[i].getLocationName();
		vector<Location> loccheck = this->locations;
		for (size_t j = 0; j < loc.size(); j++)
		{
			if (i == j)
				break ;
			if (!locationName.compare(loccheck[j].getLocationName()))
				throw std::invalid_argument(throwmessage(loccheck[i].line, "Error: This Location already exist in the line " + intToString(loccheck[i].line) +"."));
		}
		loccheck.clear();
	}
}



void	server::Myserver_name(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in server_name."));
	this->serverName = withoutsemicolon(list[1]);
}



void	server::Myroot(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide root path."));
	this->root = withoutsemicolon(list[1]);
}

void	server::Myindex(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide index path."));
	this->index = withoutsemicolon(list[1]);
}

void	server::Myclient_body_limit(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide input in Client Body Limit."));
	char* endPtr;
    long parsedcbl = std::strtol(withoutsemicolon(list[1]).c_str(), &endPtr, 10);
	std::string str = withoutsemicolon(list[1]).c_str();
	if (endPtr == str.c_str() + strlen(str.c_str()) || parsedcbl < 0 || parsedcbl > static_cast<long long>(__LONG_LONG_MAX__))
		throw std::invalid_argument(throwmessage(line, "Error: Invalide input in Client Body Limit."));
    this->client_body_limit = static_cast<int>(parsedcbl);
}

void	server::Myautoindex(std::vector<std::string> list, int line){
	std::string autoin;

	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in autoindex ON/OFF."));
	autoin = withoutsemicolon(list[1]);
	if (!autoin.compare("ON"))
		this->autoindex = 1;
	else if (!autoin.compare("OFF"))
		this->autoindex = 0;
	else
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in autoindex ON/OFF."));
}

void	server::Myupload(std::vector<std::string> list, int line){
	std::string autoin;

	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in upload ON/OFF."));
	autoin = withoutsemicolon(list[1]);
	if (!autoin.compare("ON"))
		this->upload = 1;
	else if (!autoin.compare("OFF"))
		this->upload = 0;
	else
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in upload ON/OFF."));
}

void	server::Myallow_methods(std::vector<std::string> list, int line){
	std::vector<std::string> allows;
	std::string	method;

	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() == 1 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in allows methodes."));
	for (size_t i = 1; i < list.size(); i++)
	{
		method = withoutsemicolon(list[i]);
		if (method.compare("GET") && method.compare("POST") && method.compare("DELETE"))
			throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in allows methodes."));
		allows.push_back(method);
	}
	if (allows.empty() || allows.size() > 3)
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in allows methodes."));
	this->allow_methods = allows;
}

void	server::Mycgi(std::vector<std::string> list, int line){
	std::pair<string, string> cgi;

	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 3 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in CGI."));
	cgi.first = withoutsemicolon(list[1]);
	cgi.second = withoutsemicolon(list[2]);
	for (size_t i = 0; i < this->cgi_exe.size(); i++)
	{
		if (this->cgi_exe[i].second == cgi.second)
			throw std::invalid_argument(throwmessage(line, "Error: This CGI is already exist."));
	}
	this->cgi_exe.push_back(cgi);
}

void	server::Myerror_page(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 3 || list.empty())
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Input in Error_Page."));
	char* endPtr;
	string s = withoutsemicolon(list[1]);
	const char* str = s.c_str();
    long errornbr = std::strtol(str, &endPtr, 10);
	if (*endPtr != '\0' || errornbr > static_cast<long>(INT_MAX))
		throw std::invalid_argument(throwmessage(line, "Error: Invalide Error Code."));
	std::string path = withoutsemicolon(list[2]);
	this->error_page[errornbr] = path;
}

void	server::Mylocations(std::vector<Location>& sname){
	this->locations = sname;
}


//=============== seters ===================//

void	server::setPort(std::string str){
	this->port = str;
}

void	server::setIp(std::string str){
	this->ip = str;
}

void	server::setServerName(std::string str){
	this->serverName = str;
}

void	server::setRoot(std::string str){
	this->root = str;
}

void	server::setIndex(std::string str){
	this->index = str;
}

void	server::setClientBodyLimit(int nbr){
	this->client_body_limit = nbr;
}

void	server::setAutoindex(bool b){
	this->autoindex = b;
}

void	server::setAllowMethods(std::string method){
	this->allow_methods.push_back(method);
}

void	server::setErrorPage(int error, std::string path){
	this->error_page[error] = path; 
}

void	server::setLocations(Location& obj){
	this->locations.push_back(obj);
}

void	server::setUpload(bool b){
	this->upload = b;
}

void	server::setCgiExe(std::vector< std::pair<std::string, std::string> > cgi){
	this->cgi_exe = cgi;
}

void	server::setSmSoServers(server& srv){
	this->smSoServers.push_back(srv);
}

//=============== geters ===================//
std::string	server::getIndex(void) const{
	return this->index;
}

std::string	server::getRoot(void) const{
	return this->root;
}

std::string	server::getServerName(void) const{
	return this->serverName;
}

std::string	server::getPort(void) const{
	return this->port;
}

std::string	server::getIp(void) const{
	return this->ip;
}

int	server::getClientBodyLimit(void) const{
	return this->client_body_limit;
}

bool	server::getAutoindex(void) const{
	return this->autoindex;
}

std::map<int, std::string>	server::getErrorPage(void) const{
	return this->error_page;
}

std::vector< std::string>	server::getAllowMethods(void) const{
	return this->allow_methods;
}

std::vector<Location>	server::getLocations(void) const{
	return this->locations;
}

bool	server::getUpload(void) const{
	return this->upload;
}

std::vector< std::pair<std::string, std::string> > server::getCgiExe(void) const{
	return this->cgi_exe;
}

std::vector< server > server::getSmSoServers(void) const{
	return this->smSoServers;
}