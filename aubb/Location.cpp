#include "Location.hpp"

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::vector<std::string> Location::splitString(const std::string& input, const std::string& delm) {
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


std::string Location::throwmessage(int number, const std::string& str) {
    std::stringstream ss;
    ss << number;
    return  "config:line:" + ss.str() + ": " + str;
}

void	Location::checkfirstline(std::string str, int line){
	std::vector<std::string> list;
	list = splitString(str, " \t");
	if (list.empty() || list[0].compare("location") || list.size() != 3 || list[2].compare("("))
		throw std::invalid_argument(throwmessage(line, "invalid input."));
	if (str[0] != '\t' || std::isspace(str[1]))
		throw std::invalid_argument(throwmessage(line, "execept one TAB at the start."));
	if (std::isspace(str[str.length() - 1]))
		throw std::invalid_argument(throwmessage(line, "extra whitespaces at the end."));
	setLocationName(list[1]);
	if (this->locationName[0] != '/')
		throw std::invalid_argument(throwmessage(line, "Error: Location name have to start with '/'."));
}

void	Location::checklastline(std::string str, int line, int firstline){
	if (str.empty())
		throw std::invalid_argument(throwmessage(firstline, "Error: Invalid Input, must close your location with bracket ')' ."));
	if (str[0] != '\t' || std::isspace(str[1]))
        throw std::invalid_argument(throwmessage(line, "Error: execept one TAB at the start."));
	if (std::isspace(str[str.length() - 1]))
		throw std::invalid_argument(throwmessage(line, "Error: extra whitespaces at the end."));
	if (str.compare("\t)"))
		throw std::invalid_argument(throwmessage(line, "Error: Invalid Input"));
}

bool Location::isInMyList(const std::string& target, int &token) {
	std::vector<std::string> lst;
    lst.push_back("root");
    lst.push_back("index");
    lst.push_back("autoindex");
    lst.push_back("allow_methods");
    lst.push_back("cgi_path");
    lst.push_back("cgi_extension");
	for (size_t i = 0; i < lst.size(); i++){
		if (lst[i].compare(target) == 0){
			token = i + 1;
			break;
		}
	}
	return std::find(lst.begin(), lst.end(), target) != lst.end();
}

bool Location::isWhitespace(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!std::isspace(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}

std::string Location::withoutsemicolon(std::string str){
	if (!str.empty()) {
        size_t lastNonSemicolon = str.find_last_not_of(';');
        if (lastNonSemicolon != std::string::npos && lastNonSemicolon < str.length() - 1) {
            return str.substr(0, lastNonSemicolon + 1);
        }
    }

    return str;
}


void	Location::Myroot(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide root path."));
	setRoot(withoutsemicolon(list[1]));
	this->r = 1;
}

void	Location::Myindex(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide index path."));
	setIndex(withoutsemicolon(list[1]));
	this->i = 1;
}


void	Location::Myautoindex(std::vector<std::string> list, int line){
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
	this->a = 1;
}


void	Location::Myallow_methods(std::vector<std::string> list, int line){
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
	this->am = 1;
}

void	Location::Mycgi_path(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide index path."));
	setCgiPath(withoutsemicolon(list[1]));
}

void	Location::Mycgi_extension(std::vector<std::string> list, int line){
	if (!list[list.size() - 1].compare(";"))
		list.pop_back();
	if (list.size() != 2 || list.empty())
			throw std::invalid_argument(throwmessage(line, "Error: Invalide index path."));
	setCgiExtension(withoutsemicolon(list[1]));
}

void	Location::set_value(std::vector<std::string> list, int token, int line){
	switch (token)
	{
	case 1:
		Myroot(list, line);
		break;
	case 2:
		Myindex(list, line);
		break;
	case 3:
		Myautoindex(list, line);
		break;
	case 4:
		Myallow_methods(list, line);
		break;
	case 5:
		Mycgi_path(list, line);
		break;
	case 6:
		Mycgi_extension(list, line);
		break;
	default:
		break;
	}
}

void	Location::seter(std::string str, int line){
	std::vector<std::string> list;
	int	token;

	list = splitString(str, " \t");
	if (list.empty() || list[0][0] == '#')
		return ;
	if (!isInMyList(list[0], token))
		throw std::invalid_argument(throwmessage(line, "invalid input."));
	if (list[list.size() - 1][list[list.size() - 1].length() - 1] != ';')
		throw std::invalid_argument(throwmessage(line, "the line have to end with ';'."));
	int f = 0;
	for (size_t i = str.size() - 1; i > 0; i--){
		if (str[i] == ';')
			f++;
		if (f == 2)
			throw std::invalid_argument(throwmessage(line, "Error: More than one semicolon in the string."));
	}
	if (str[0] != '\t' || str[0] != '\t'|| std::isspace(str[2]))
		throw std::invalid_argument(throwmessage(line, "execept two TAB at the start."));
	set_value(list, token, line);
}

void	Location::init(){
	setAutoindex(0);
	setLocationName("/");
	setPath("/public/");
	setRoot("/public");
	setIndex("");
	setAutoindex(0);
	setAllowMethods("GET");
	setAllowMethods("POST");
	setAllowMethods("DELETE");
	this->a = 0;
	this->am = 0;
	this->i = 0;
	this->r = 0;
}


std::string rmSlash(const std::string& str) {

	// Check if the input string contains only "/"
    if (str.find_first_not_of('/') == std::string::npos) {
        return "";  // Return an empty string
    }

    std::string modifiedString = str;

    // Remove trailing slashes
    size_t lastNonSlash = modifiedString.find_last_not_of('/');
    if (lastNonSlash != std::string::npos && lastNonSlash < modifiedString.length() - 1) {
        modifiedString.erase(lastNonSlash + 1);
    }

    // Remove leading slashes
    size_t firstNonSlash = modifiedString.find_first_not_of('/');
    if (firstNonSlash != std::string::npos && firstNonSlash > 0) {
        modifiedString.erase(0, firstNonSlash);
    }

    return modifiedString;
}

void	Location::pathset(){
	if (rmSlash(getLocationName()).empty())
		this->path = "/" + rmSlash(getRoot()) + "/";
	else
		setPath("/" + rmSlash(getRoot()) + "/" + rmSlash(getLocationName()) + "/");
}

Location::Location(string root, string index, bool autoindex, vector<string> allowMethods){
	init();
	if (!root.empty())
		setRoot(root);
	if (!index.empty())
		setIndex(index);
	if (!allowMethods.empty())
		this->allow_methods = allowMethods;
	setAutoindex(autoindex);
	pathset();
}

Location::Location(std::map<int, std::string>& location){
	init();
	std::map<int, std::string>::const_iterator it = location.begin();
	std::map<int, std::string>::reverse_iterator rit = location.rbegin();

	checkfirstline(it->second, it->first);
	checklastline(rit->second, rit->first, it->first);
	it++;
	for (; it != location.end(); it++)
    {
		if (it->second.find(")") == std::string::npos){
       		seter(it->second, it->first);
		}
	}
	pathset();
}


std::string& Location::operator[](const std::string& key) {
	if (key == "root")
		return root;
	else if (key == "path")
		return path;
	else if (key == "index")
		return index;
	else if (key == "cgi_path")
		return cgi_path;
	else if (key == "cgi_extension")
		return cgi_extension;
	else
		return data[key];
}


//=============== seters ===================//

void	Location::setPath(std::string str){
	this->data["path"] = str;
	this->path = str;
}
void	Location::setLocationName(std::string str){
	this->data["locationName"] = str;
	this->locationName = str;
}
void	Location::setRoot(std::string str){
	this->data["root"] = str;
	this->root = str;
}
void	Location::setIndex(std::string str){
	this->data["index"] = str;
	this->index = str;
}
void	Location::setCgiPath(std::string str){
	this->data["cgi_path"] = str;
	this->cgi_path = str;
}
void	Location::setCgiExtension(std::string str){
	this->data["cgi_extension"] = str;
	this->cgi_extension = str;
}
void	Location::setAutoindex(bool b){
	this->autoindex = b;
}
void	Location::setAllowMethods(std::string str){
	this->allow_methods.push_back(str);
}

void	Location::setVecAllowMethods(std::vector< std::string> vec){
	this->allow_methods = vec;
}

//=============== geters ===================//

std::string	Location::getPath(void) const{
	return this->path;
}
std::string	Location::getLocationName(void) const{
	return this->locationName;
}
std::string	Location::getRoot(void) const{
	return this->root;
}
std::string	Location::getIndex(void) const{
	return this->index;
}
std::string	Location::getCgiPath(void) const{
	return this->cgi_path;
}
std::string	Location::getCgiExtension(void) const{
	return this->cgi_extension;
}
std::vector< std::string>	Location::getAllowMethods(void) const{
	return this->allow_methods;
}
bool		Location::getAutoindex(void) const{
	return this->autoindex;
}
