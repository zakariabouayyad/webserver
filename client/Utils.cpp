#include "../server.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

int checkExistance(request& requestObj){
    string path;
    if (requestObj.getMethod() == "DELETE")
        path = requestObj.loc.getRoot() + requestObj.getrequestURI();
	else
        path = requestObj.getFilePath();
    if (!fileExists(path.c_str())) {
		codeNpath(requestObj,"404 not found", errorPageTamplate("404, Not Found.").c_str(), requestObj.errorpages);
		return 1;
	}
	return 0;
}

bool endsWithSlash(const std::string& str) {
    if (str.length() == 0)
        return false;

    char lastChar = str[str.length() - 1];
    return lastChar == '/';
}

std::string getFileExtension(const std::string& filePath) {
    std::string::size_type dotPos = filePath.rfind('.');

    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos + 1);
    }

    return "";
}

void	isCGI(request& requestObj, const std::string& filePath,
	server& _server){
	std::string exe = getFileExtension(filePath);
	vector< std::pair<string, string> > cgi_exe = _server.getCgiExe();
	for (size_t i = 0; i < cgi_exe.size(); i++)
	{
		if (exe == cgi_exe[i].second)
		{
			requestObj.is_CGI = 1;
			requestObj.cgi_exe = cgi_exe[i];
            requestObj.CgiObj = new Cgi(&_server, &requestObj);
			requestObj.CgiObj->exe();
            requestObj.Cgisdone = 1;
			break;
		}
	}
}

void generateAutoIndex(const std::string& directoryPath, const std::string& outputFileName) {
    DIR* dir = opendir(directoryPath.c_str());

    if (!dir) {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return;
    }

    if (fileExists(outputFileName.c_str()))
        unlink(outputFileName.c_str());

    std::ofstream outputFile(outputFileName.c_str());

    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << strerror(errno) << std::endl;
        closedir(dir);
        return;
    }

    outputFile << "<!DOCTYPE html>\n<html>\n<head>\n\t<title>Index Page</title>\n\t<style>\n";
    outputFile << "\t\tbody {\n\t\t\tfont-family: Arial, sans-serif;\n\t\t\tbackground-color: #f2f2f2;\n";
    outputFile << "\t\t\tmargin: 0;\n\t\t\tpadding: 0;\n\t\t}\n\t\t.container {\n\t\t\tmax-width: 600px;\n";
    outputFile << "\t\t\tmargin: 0 auto;\n\t\t\tpadding: 20px;\n\t\t}\n\t\t.title {\n";
    outputFile << "\t\t\tfont-family: Georgia;\n\t\t\tfont-size: 1.0rem;\n\t\t\tcolor: #0e2431;\n";
    outputFile << "\t\t\tborder-bottom: 1px solid #ccc;\n\t\t}\n\t\th1 {\n\t\t\ttext-align: center;\n";
    outputFile << "\t\t\tcolor: #333;\n\t\t}\n\t\tul {\n\t\t\ttext-align: center;\n\t\t\tlist-style-type: none;\n";
    outputFile << "\t\t\tpadding: 0;\n\t\t}\n\t\tli {\n\t\t\tmargin-bottom: 15px;\n\t\t}\n\t\ta {\n";
    outputFile << "\t\t\ttext-decoration: none;\n\t\t\tcolor: #333;\n\t\t}\n\t\ta:hover {\n";
    outputFile << "\t\t\tfont-weight: bold;\n\t\t}\n\t</style>\n</head>\n<body>\n\t<div class=\"title\">\n";
    outputFile << "\t\t<h1>Index Page</h1>\n\t</div>\n\t<div class=\"container\">\n\t\t<ul>\n";

    // Read directory contents
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        std::string entryName = entry->d_name;

        // Skip current and parent directory entries
        if (entryName != "." && entryName != "..") {
            // Create links for each entry
            outputFile << "\t\t\t<li><a href=\"" << entryName << "\">" << entryName << "</a></li>\n";
        }
    }

    outputFile << "\t\t</ul>\n\t</div>\n</body>\n</html>\n";

    outputFile.close();
    closedir(dir);

    std::cout << "Autoindexing completed. Output file: " << outputFileName << std::endl;
}

int extractStatusCode(const std::string& response) {
	std::istringstream iss(response);

	int statusCode;
	iss >> statusCode;

	return statusCode;
}

void	codeNpath(request& requestObj, string statusCode, string filePath, map<int, string> errPages){
	int key = extractStatusCode(statusCode);

	requestObj.setStatusCode(statusCode);
	requestObj.setContentType("text/html");
	if (errPages.find(key) != errPages.end()) {
		if (access(errPages[key].c_str(), R_OK) == 0) {
			requestObj.setFilePath(errPages[key]);
		}
		else {
			requestObj.setFilePath(filePath);
		}
	}
	else {
		requestObj.setFilePath(filePath);
	}
    requestObj.setmethod("GET");
    requestObj.is_CGI = 0;
}

void internalServerError(int connection_socket){
	std::string __response = "HTTP/1.1 500 Internal Server Error\n"
					"Content-Length: 788\n"
					"Content-Type: text/html\r\n\r\n"
		 "<!DOCTYPE html>\n"
		 "<html>\n"
			 "<head>\n"
				 "<style>\n"
					 "html, body {font-family: 'Roboto Mono', monospace;font-size: 16px;}\n"
					 "body {background-color: black;margin: 0;padding: 0;}\n"
					 "p {color: white;font-size: 25px;letter-spacing: 0.2px;margin: 0;display: inline;}\n"
					 ".center-xy {text-align: center;top: 50%;left: 50%;transform: translate(-50%, -50%);position: absolute;}\n"
				 "</style>\n"
			 "</head>\n"
			 "<body>\n"
				 "<div class='center-xy'>\n"
					 "<p id='myP'>\n"
						 "500 Internal Server Error\n"
					 "</p>\n"
				 "</div>\n"
				 "<script>\n"
			"let divElement = document.getElementById(\"myP\");"
			"let textContent = divElement.innerText.toString();"
			"let i = 1;"
			"function typeWriter() {"
			"divElement.innerText = textContent.slice(0, i);"
			"console.log(divElement.innerText);"
			"i++;"
			"if (i <= textContent.length)"
			"setTimeout(typeWriter, 100);"
			"}"
			"setTimeout(typeWriter, 0);\n"
				 "</script>\n"
			 "</body>\n"
		 "</html>";
	write(connection_socket, __response.c_str(), strlen(__response.c_str()));
}