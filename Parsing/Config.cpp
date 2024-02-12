#include "Config.hpp"

Config::Config() {
}

Config::~Config(){
}

std::string Config::throwmessage(int number, const std::string& str) {
	std::stringstream ss;
	ss << number;
	return  "config:line:" + ss.str() + ": " + str;
}

bool Config::check_ext(){
	if (path_name.length() < 5)
		return true;
	if (path_name.compare(path_name.length() - 5, 5, ".conf"))
		return true;
	return false;
}

void	Config::dublesrvcheck(){
	std::vector<server> srv = this->Servers;
	std::string _port, _ip, _serverName;
	for (size_t i = 0; i < srv.size(); i++){
		_port = srv[i].getPort();
		_ip = srv[i].getIp();
		_serverName = srv[i].getServerName();
		std::vector<server> srvcheck = this->Servers;
		for (size_t j = 0; j < srvcheck.size(); j++){
			if (i == j)
				break ;
			if (!_serverName.compare(srvcheck[j].getServerName()) && !_port.compare(srvcheck[j].getPort())
				&& (!_ip.compare(srvcheck[j].get_ip()) || (_ip == "localhost" && srvcheck[j].get_ip() == "127.0.0.1")
				|| (_ip == "127.0.0.1" && srvcheck[j].get_ip() == "localhost")))
				throw std::invalid_argument(throwmessage(srv[i].line, "Error: This Server already exist in the line " + intToString(srvcheck[j].line) +"."));
		}
		srvcheck.clear();
	}
}


void	Config::defaultCheck(){

	std::vector<server> &srv = this->Servers;
	std::string _port, _ip, _serverName;
	for (size_t i = 0; i < srv.size(); i++){
		_port = srv[i].getPort();
		_ip = srv[i].getIp();
		_serverName = srv[i].getServerName();
		std::vector<server> srvcheck = this->Servers;
		for (size_t j = 0; j < srvcheck.size(); j++){
			if (i == j)
				break ;
			if (!_port.compare(srvcheck[j].getPort()) && (!_ip.compare(srvcheck[j].get_ip()) || (_ip == "localhost" && srvcheck[j].get_ip() == "127.0.0.1")
				|| (_ip == "127.0.0.1" && srvcheck[j].get_ip() == "localhost"))){
				srv[i].set_isdefault(0);
				srv[i].set_my_default(j);
				break ;
			}
		}
		srvcheck.clear();
	}
}

void    Config::smSoServersInit(){
	std::vector<server> &srv = this->Servers;
	for (size_t i = 0; i < srv.size(); i++){
		if (srv[i].get_isdefault() == 0)
		{
			int mydefault = srv[i].get_my_default();
			srv[mydefault].setSmSoServers(srv[i]);
		}
	}
}

Config::Config(int ac, char **av){
	string  path;
	if (ac > 2)
		throw std::invalid_argument("Invalid Argument.");
	else if (ac == 1)
		path_name = "config.conf";
	else
		path_name = av[1];
	parse();
	dublesrvcheck();
	defaultCheck();
	smSoServersInit();
}

bool Config::isWhitespace(const std::string& str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!std::isspace(static_cast<unsigned char>(*it))) {
			return false; // Found a non-whitespace character
		}
	}
	return true; // Only whitespace characters found
}

void     Config::parse_server(std::ifstream &ConfFile){
	std::vector<std::map<int, std::string> > servers;
	std::map<int, std::string> currentServer;
	std::string line;
	int flag = 0;
	int i = 1;

	while (std::getline(ConfFile, line)) {
		// Check if the line starts a new server block
		if (line.find("{") != std::string::npos){
			if (!currentServer.empty()) {
				server  srv;
				srv.parse(currentServer);
				Servers.push_back(srv);
				currentServer.clear();
			}
			currentServer[i] = line;
			flag = 1;
		} 
		else if(line.find("}") != std::string::npos){
			currentServer[i] = line;
			if (!currentServer.empty()) {
				server  srv;
				srv.parse(currentServer);
				Servers.push_back(srv);
				currentServer.clear();
			}
			flag = 0;
		} 
		else if(flag){
			// Add the line to the current server's configLines
			currentServer[i] = line;
		}
		else if (!flag){
			if (!line.empty() && !isWhitespace(line))
				throw std::invalid_argument(throwmessage(i, "Error: Unknown Input."));
		}
		i++;
	}
	// Add the last server
	if (!currentServer.empty()) {
		server  srv;
		srv.parse(currentServer);
		Servers.push_back(srv);
	}
}


void    Config::parse(){
	std::ifstream   ConfFile(this->path_name);
	std::string     line;

	if (check_ext())
		throw std::invalid_argument("server: invalid config file => file not [.conf].");
	if (!ConfFile.is_open())
		throw std::invalid_argument("server: invalid config file => file not existe.");
	if (ConfFile.peek() == std::ifstream::traits_type::eof())
		throw std::invalid_argument("server: invalid config file => file is empty.");
	parse_server(ConfFile);
}


void	Config::display(){
	std::vector<server> srv = this->Servers;
	for (size_t i = 0; i < srv.size(); i++)
	{
		std::cout << "----------------------" <<std::endl;
		std::cout << "SERVER:\t"<< i + 1 <<std::endl;
		std::cout << "server name:\t["<< srv[i].getServerName() << "]" << std::endl;
		std::cout << "port:\t\t["<< srv[i].getPort() << "]" << std::endl;
		std::cout << "hostname:\t["<<  srv[i].getIp() << "]" << std::endl;
		std::cout << "root path:\t["<<  srv[i].getRoot() << "]" << std::endl;
		std::cout << "is default:\t["<<  srv[i].get_isdefault() << "]" << std::endl;
		std::cout << "my default:\t["<<  srv[i].get_my_default() << "]" << std::endl;
		std::cout << "autoindedx:\t["<<  srv[i].getAutoindex() << "]" << std::endl;
		std::cout << "indedx:\t\t["<<  srv[i].getIndex() << "]" << std::endl;
		std::cout << "CBL:\t\t["<<  srv[i].getClientBodyLimit() << "]" << std::endl;
		std::cout << "Uplad:\t\t["<<  srv[i].getUpload() << "]" << std::endl;
		const std::map<int, std::string>& errorPageMap = srv[i].getErrorPage();
		for (std::map<int, std::string>::const_iterator it = errorPageMap.begin(); it != errorPageMap.end(); ++it){
			std::cout << "Error_page:\t["<<  it->first << "]"<< " [" << it->second << "]" << std::endl;
		}
		std::cout << "Allow Methods:\t[ " ;
		const std::vector<std::string>& allows = srv[i].getAllowMethods();
		for (size_t i = 0; i < allows.size(); i++){
			std::cout << allows[i]<< " ";
		}
		std::cout << "]"<<std::endl;
		const std::vector< std::pair<std::string, std::string> >& cgi = srv[i].getCgiExe();
			for (size_t i = 0; i < cgi.size(); i++){
				std::cout << "cgi path:\t[" << cgi[i].first << "] ext[" << cgi[i].second << "]"<< std::endl;
			}
		std::vector<Location> local = srv[i].getLocations();
		for (size_t i = 0; i < local.size(); i++){
			std::cout << "Local: " << i + 1 << std::endl;
			std::cout << "\tlocation name:\t[" << local[i].getLocationName() << "]" << std::endl;
			std::cout << "\tpath:\t\t[" << local[i].getPath() << "]" << std::endl;
			std::cout << "\troot:\t\t[" << local[i].getRoot() << "]" << std::endl;
			std::cout << "\tindex:\t\t[" << local[i].getIndex() << "]" << std::endl;
			std::cout << "\tautoindex:\t[" << local[i].getAutoindex() << "]" << std::endl;
			std::cout << "\tupload:\t\t[" << local[i].getUpload() << "]" << std::endl;
			std::cout << "\treturn:\t\t[" << local[i].getRedirection() << "]" << std::endl;
			std::cout << "\tAllow Methods:\t[ " ;
			const std::vector<std::string>& allowss = local[i].getAllowMethods();
			for (size_t i = 0; i < allowss.size(); i++){
				std::cout << allowss[i]<< " ";
			}
			std::cout << "]"<<std::endl;
			const std::vector< std::pair<std::string, std::string> >& cgi = local[i].getCgiExe();
			for (size_t i = 0; i < cgi.size(); i++){
				std::cout << "\tcgi path:\t[" << cgi[i].first << "] ext[" << cgi[i].second << "]"<< std::endl;
			}
		}
		std::vector<server> smsosrv = srv[i].getSmSoServers();
		for (size_t i = 0; i < smsosrv.size(); i++){
			std::cout << "Smsosrv: " << i + 1 << std::endl;
			std::cout << "\tserver name:\t["<< smsosrv[i].getServerName() << "]" << std::endl;
			std::cout << "\tport:\t\t["<< smsosrv[i].getPort() << "]" << std::endl;
			std::cout << "\thostname:\t["<<  smsosrv[i].getIp() << "]" << std::endl;

		}
	}
}
