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
				&& !_ip.compare(srvcheck[i].get_ip()))
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
			if (_serverName.compare(srvcheck[j].getServerName()) && !_port.compare(srvcheck[j].getPort())
				&& !_ip.compare(srvcheck[i].get_ip())){
				srv[i].set_isdefault(0);
				srv[i].set_my_default(j);
				break ;
			}
		}
		srvcheck.clear();
	}
}

Config::Config(std::string av): path_name(av){
    parse();
	dublesrvcheck();
	defaultCheck();
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
