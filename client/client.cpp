#include "../server.hpp"
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

bool endsWithSlash(const std::string& str) {
    if (str.length() == 0)
        return false;

    char lastChar = str[str.length() - 1];
    return lastChar == '/';
}

void generateAutoIndex(const std::string& directoryPath, const std::string& outputFileName) {
    DIR* dir = opendir(directoryPath.c_str());

    if (!dir) {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return;
    }

    std::ofstream outputFile(outputFileName.c_str());

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << strerror(errno) << std::endl;
        closedir(dir);
        return;
    }

    // Write HTML header
    outputFile << "<html><head><title>Index of " << directoryPath << "</title></head><body>\n";
    outputFile << "<h2>Index of " << directoryPath << "</h2>\n";
    outputFile << "<ul>\n";

    // Read directory contents
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        std::string entryName = entry->d_name;

        // Skip current and parent directory entries
        if (entryName != "." && entryName != "..") {
            // Create links for each entry
            outputFile << "<li><a href=\"" << entryName << "\">" << entryName << "</a></li>\n";
        }
    }

    // Write HTML footer
    outputFile << "</ul></body></html>\n";

    outputFile.close();
    closedir(dir);

    std::cout << "Autoindexing completed. Output file: " << outputFileName << std::endl;
}

std::string getFileExtension(const std::string& filePath) {
    std::string::size_type dotPos = filePath.rfind('.');

    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos);
    }

    return "";
}

void	requestCases(request &requestObj, server& _server)
{
	if (requestObj.getMethod() == "GET") {
		if (!fileExists(requestObj.getFilePath().c_str()) && !isDirectory(requestObj.getFilePath().c_str())) {
			requestObj.setStatusCode(404);
			requestObj.setFilePath(errorPageTamplate("404, Not Found."));
			return ;
		}
		if (isDirectory(requestObj.getFilePath().c_str()))
		{
			if (!endsWithSlash(requestObj.getFilePath()))
			{
				requestObj.setStatusCode(301);
				requestObj.setFilePath(requestObj.getFilePath() + "/");
				return ;
			}
			if (!_server.getIndex().empty()) {
				if (!_server.getAutoindex()) {
					requestObj.setStatusCode(403);
					requestObj.setFilePath(errorPageTamplate("403, Forbidden."));
					return ;
				}
				else {
					generateAutoIndex(requestObj.getFilePath(), "autoindex.html");//?need to do lmsa l file d index
					requestObj.setStatusCode(200);
					requestObj.setFilePath("autoindex.html");
					return ;
				}
			}
			else {
				if (getFileExtension(requestObj.getFilePath()) == ".php"
					|| getFileExtension(requestObj.getFilePath()) == ".py") {
					cout << RED << "hna dakchi f cgi" << RESET_TEXT << endl;
				}
			}
		}
		else {
			if (getFileExtension(requestObj.getFilePath()) == ".php"
				|| getFileExtension(requestObj.getFilePath()) == ".py") {
				cout << RED << "ta hna dakchi f cgi" << RESET_TEXT << endl;
			}
		}
//!if uri in get has "?" take until ?
	}
	else if (requestObj.getMethod() == "POST") {
		if (_server.getUpload()) {//? belhadj khasso ybelel upload l location 
			cout << BLUE << "UPLOAD IS ON |";
			cout << requestObj.getContentType();
			cout << "||" << RESET_TEXT << endl;
			// cout << RED << "UPLOAD IS ON " << requestObj.getContentType() << RESET_TEXT << endl;
		}
		if (!_server.getUpload())
				cout << RED << "UPLOAD IS OFF" << RESET_TEXT << endl;
		if (!fileExists(requestObj.getFilePath().c_str()) && !isDirectory(requestObj.getFilePath().c_str())) {
			requestObj.setStatusCode(404);
			requestObj.setFilePath(errorPageTamplate("404, Not Found."));
			return ;
		}
		if (isDirectory(requestObj.getFilePath().c_str()))
		{
			if (!endsWithSlash(requestObj.getFilePath()))
			{
				requestObj.setStatusCode(301);
				requestObj.setFilePath(requestObj.getFilePath() + "/");
				return ;
			}
			else {
				if (!_server.getIndex().empty()) {
					requestObj.setStatusCode(403);
					requestObj.setFilePath(errorPageTamplate("403, Forbidden."));
					return ;
				}
				else {
					if (getFileExtension(requestObj.getFilePath()) == ".php"
						|| getFileExtension(requestObj.getFilePath()) == ".py") {
						cout << RED << "hna dakchi f cgi" << RESET_TEXT << endl;
					}
				}
			}
		}
		else {
			if (getFileExtension(requestObj.getFilePath()) == ".php"
				|| getFileExtension(requestObj.getFilePath()) == ".py") {
				cout << RED << "ta hna dakchi f cgi" << RESET_TEXT << endl;
			}
		}
	}
	else if (requestObj.getMethod() == "DELETE") {

	}
}

void	client::set_request(string r, server& _server){
	std::ofstream outputFile("debugBody", std::ios::app);
		if (outputFile.is_open()) {
			outputFile << r;
			outputFile << "\n___________________________________\n";
			outputFile.close();
		}
		cout << "tookrequest is " << tookrequest<< endl;
    tookrequest = requestObj.parseRequest(r, _server);
	
    if (tookrequest == 1) {
		cout << "tookrequest is 1" << endl;
		requestObj.matchLocation(_server);
		requestCases(requestObj, _server);
		cout << RED<< "|" << requestObj.getFilePath() << "|" << RESET_TEXT << endl;
		responseObj.totalSent = 0;
		responseObj.initialize(requestObj);
	}
}

void	client::set_response(int connection_socket){
	if (!responseObj.totalSent)
		responseObj.sendHeader(connection_socket, requestObj);
	filesent = responseObj.sendBody(connection_socket, requestObj);
	// cout << RED <<"filesent: " << filesent << RESET_TEXT << endl;
	if (filesent == 1){
	    tookrequest = 0;
	    // cout << BLUE<< "tookrequest is set to 0 again" << RESET_TEXT << endl;
	}
}

string client::getresponse(){
    return responsestring;
}

bool	client::getfilesent(){
    return filesent;
}

bool	client::getTookrequest(){
    return tookrequest;
}