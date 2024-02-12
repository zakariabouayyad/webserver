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
#include "Parsing/Location.hpp"
#include "Parsing/Config.hpp"
#include "cgi/Cgi.hpp"
#include <ctime>


using std::map;
using std::vector;
using std::string;
using std::pair;
class request;
//utils
int		checkExistance(request&);
bool	endsWithSlash(const std::string&);
void	isCGI(request&, const std::string&, server&);
string	getFileExtension(const std::string&);
void	generateAutoIndex(const std::string&, const std::string&);
void	codeNpath(request&, string, string, map<int, string>);
int		extractStatusCode(const std::string& response);
void	internalServerError(int connection_socket);
//
class client
{
	request requestObj;
	response responseObj;
	// cgi cgiOBJ;
	string responsestring;
	int		filesent;
	public:

		enum requestStatus {
			starting,
			requestLineState,
			headerFieldState,
			headersDoneState,
			bodyState,
		};
		map<int, string> errorpages;
		bool keepAlive;
		bool	tookrequest;
		time_t resTime;
		client(){
			keepAlive = 0;
			filesent = 0;
			tookrequest = 0;
			resTime = -1;
		}
		void	set_request(std::string, server&);
		int		set_response(int);
		bool	getfilesent();
		bool	getTookrequest();
		void	setTookrequest(bool t){
			tookrequest = t;
		}
		string getresponse();
		void	reset();
		//Client class
		void	_delete_(request&);
		void	_get_(request&, server&);
		void	_post_(request&, server&);
		void	requestCases(request &requestObj, server& _server);
	int		cgisdone;
};
//
class server{
	string	port, serverName, request, response, ip;
	bool is_default;
	int my_default, _s_listener, _s_connection;
	// ayoub ---------mnhna ltht(private)
	string				root;
	string				index;
	long long			client_body_limit;
	bool				autoindex;
	bool				upload;
	vector<string>		allow_methods;
	map<int,string>		error_page;
	vector< std::pair<string, string> >	cgi_exe;
	vector<server>		smSoServers;
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
	void	Myupload(std::vector<std::string> list, int line);
	void	Mycgi(std::vector<std::string> list, int line);
	void	doublelocationcheck();
	//================
	public:
		vector<Location>	locations;
		int					line;
		/*AMINE'S*/
		vector<int>		mysockets;
		map<int, client>	clients;
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
		void	setUpload(bool);
		void	setCgiExe(std::vector< std::pair<std::string, std::string> >);
		void	setSmSoServers(server &srv);

		std::string	getServerName(void) const;
		std::string	getPort(void) const;
		std::string	getIp(void) const;
		std::string	getRoot(void) const;
		std::string	getIndex(void) const;
		int			getClientBodyLimit(void) const;
		bool		getAutoindex(void) const;
		bool		getUpload(void) const;
		std::vector< std::string>		getAllowMethods(void) const;
		std::map<int, std::string>		getErrorPage(void) const;
		std::vector<Location>			getLocations(void) const;
		std::vector< std::pair<std::string, std::string> > getCgiExe(void) const;
		std::vector<server>				getSmSoServers(void) const;
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

#endif