#ifndef SERVER_HPP
#define SERVER_HPP

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET_TEXT "\033[0m"

#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "request/request.hpp"
#include "response/response.hpp"
#include "aubb/Location.hpp"
#include "aubb/Config.hpp"


using std::map;
using std::vector;
using std::string;
using std::pair;

class server{
	string	port, serverName, request, response, ip;
	bool is_default;
	int my_default, _s_listener, _s_connection;
	// ayoub ---------mnhna ltht(private)
	string				root;
	string				index;
	int					client_body_limit;
	bool				autoindex;
	vector<string>		allow_methods;
	map<int,string>		error_page;

	//===============
	void	checkfirstline(std::string str, int line);
	void	checklastline(std::string str, int line, int firstline);
	void	seter(std::string str, int line);
	void	set_value(std::vector<std::string> list, int token, int line);
	void	Myhostport(std::vector<std::string> list, int line);
	std::vector<std::string> splitString(const std::string& input, const std::string& delm) ;
	std::string throwmessage(int number, const std::string& str);
	std::string withoutsemicolon(std::string str);
	bool 	isInMyList(const std::string& target, int &token);
	bool 	isWhitespace(const std::string& str);
	void	Myserver_name(std::vector<std::string> list, int line);
	void	Myroot(std::vector<std::string> list, int line);
	void	Myindex(std::vector<std::string> list, int line);
	void	Myclient_body_limit(std::vector<std::string> list, int line);
	void	Myautoindex(std::vector<std::string> list, int line);
	void	Myallow_methods(std::vector<std::string> list, int line);
	void	Myerror_page(std::vector<std::string> list, int line);
	void	Mylocations(std::vector<Location>&);
	void	setmylocation(std::map<int, std::string>::const_iterator &it, std::map<int, std::string> &server);
	//================
	public:
		vector<Location>	locations;
		int					line;
		/*AMINE'S*/
		int				tempsm3;
		vector<int>		serversockets;
		vector<int>		connectionsockets;
		void	portSetter(string prt);
		void	set_my_default(int index);
		void	set_isdefault(bool b);
		void	set_slistener(int s);
		void	set_sconnection(int s);
		void	set_request(string rq);
		void	set_response(string rs);
		void	set_ip(string ip);
		string	get_request();
		string	get_response();
		string	portGetter();
		string	get_ip();
		bool	get_isdefault();
		int		get_my_default();
		int		get_slistener();
		int		get_sconncetion();
		/*END OF AMINE'S*/
		//===========================
		void	setPort(std::string);
		void	setIp(std::string);
		void	setServerName(std::string);
		void	setRoot(std::string);
		void	setIndex(std::string);
		void	setClientBodyLimit(int);
		void	setAutoindex(bool);
		void	setAllowMethods(std::string);
		void	setErrorPage(int, std::string);
		void	setLocations(Location&);

		std::string	getServerName(void) const;
		std::string	getPort(void) const;
		std::string	getIp(void) const;
		std::string	getRoot(void) const;
		std::string	getIndex(void) const;
		int			getClientBodyLimit(void) const;
		bool		getAutoindex(void) const;
		std::vector< std::string>		getAllowMethods(void) const;
		std::map<int, std::string>		getErrorPage(void) const;
		std::vector<Location>			getLocations(void) const;
		void	parse(std::map<int, std::string>&);
		void	init();
};

class serversInfos
{
	private:
		vector<server>			servers;
	public:
		vector<int> allSockets;
		serversInfos(const vector<server>& servers);
		void		SetListener();
		void		closeListeners();
		vector<server> get_servers();
};

void	main_loop(vector<server>);

//REQUEST_CLASS
// class request
// {
// 	string method;
// 	string requestURI;
// 	string httpVersion;
// 	map<std::string, std::string> headerFields;
// 	string body;

// public:
// 	request();
// 	request(std::string req);
// 	request(const request &other);
// 	request& operator=(const request& other);
// 	std::string getMethod();
// 	void checkRequestLine(std::string request);
// 	void checkHeaderFields(std::string headerFiles);
// 	void parseRequest(std::string request);
// 	void checkBody(std::string body);
// 	string getrequestURI();
// 	string getContentType();
// };

//RESPONSE_CLASS
// class response{
// 	// string http_version, status_code;
// 	// map<string, string> header_fields;
// 	// string	body;
// 	string uri, contentType;
// 	string res;
// 	public:
// 		void	set_res();
// 		string	get_res();
// 		void	setURI(string uri);
// 		void	setcontentType(string contenttype);
// };

//Client class
class client
{
	request requestObj;
	response responseObj;
	string responsestring;
	bool	filesent;
	bool	tookrequest;
	public:
		client(){
			filesent = 0;
			tookrequest = 0;
		}
		void	set_request(char*, server&);
		void	set_response(int);
		bool	getfilesent();
		bool	getTookrequest();
		string getresponse();
};


#endif