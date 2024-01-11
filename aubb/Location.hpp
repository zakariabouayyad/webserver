#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sstream>

using std::map;
using std::vector;
using std::string;
using std::pair;

class Location
{
private:
	std::string					path;
	std::string					locationName;
	std::string					root;
	std::string					index;
	bool						autoindex;
	std::vector< std::string>	allow_methods;
	std::string					cgi_path;
	std::string					cgi_extension;

	std::map<std::string, std::string> data;

	void	init();
	std::string throwmessage(int number, const std::string& str);
	void	checkfirstline(std::string str, int line);
	std::vector<std::string> splitString(const std::string& input, const std::string& delm);
	void	checklastline(std::string str, int line, int firstline);
	void	seter(std::string str, int line);
	bool	isInMyList(const std::string& target, int &token);
	bool	isWhitespace(const std::string& str);
	void	set_value(std::vector<std::string> list, int token, int line);
	std::string	withoutsemicolon(std::string str);
	void	Mycgi_extension(std::vector<std::string> list, int line);

	void	Myroot(std::vector<std::string> list, int line);
	void	Myindex(std::vector<std::string> list, int line);
	void	Myautoindex(std::vector<std::string> list, int line);
	void	Myallow_methods(std::vector<std::string> list, int line);
	void	Mycgi_path(std::vector<std::string> list, int line);
	void	pathset();
public:
	int	r, i, a, am;
	Location(){}
	Location(std::map<int, std::string>&);
	Location(string root, string index, bool autoindex, vector<string> allowMethods);
	std::string& operator[](const std::string& key);
	void	setPath(std::string);
	void	setLocationName(std::string);
	void	setRoot(std::string);
	void	setIndex(std::string);
	void	setCgiPath(std::string);
	void	setCgiExtension(std::string);
	void	setAutoindex(bool);
	void	setAllowMethods(std::string);
	void	setVecAllowMethods(std::vector< std::string>);

	std::string	getPath(void) const;
	std::string	getLocationName(void) const;
	std::string	getRoot(void) const;
	std::string	getIndex(void) const;
	std::string	getCgiPath(void) const;
	std::string	getCgiExtension(void) const;
	bool		getAutoindex(void) const;
	std::vector< std::string>	getAllowMethods(void) const;
};

std::string intToString(int value);
#endif