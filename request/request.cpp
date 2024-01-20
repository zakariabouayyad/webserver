#include "../server.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

request::request() : requestStatus(true) {
	this->chunkSize = 0;
	this->bodyContentLength = 0;
	this->flag = 0;
	this->gg = 0;
}

// request::request()
// {
//	 this->chunkSize = 0;
//	 // this->_request = _request;
//	 // this->_server = _server;
//	 // parseRequest(request, _server);
// }

request::request(const request &other)
{
	*this = other;
}

request& request::operator=(const request& other)
{
	this->method = other.method;
	this->requestURI = other.requestURI;
	return *this;
}

void printError(std::string errorMsg, int status)
{
	std::cerr << RED << errorMsg << RESET_TEXT << std::endl;
	exit(status);
}

std::string request::getMethod() {
	return this->method;
}

string request::getrequestURI(){
	return requestURI;
}

string request::getContentType(){
	return this->ContentType;
}

std::string request::getHttpVersion() {
	return this->httpVersion;
}

std::string request::getFilePath() {
	return this->filePath;
}

void request::setFilePath(std::string filePath) {
	this->filePath = filePath;
}

bool	request::isRequestDone() {
	return requestStatus;
}

int request::getStatusCode() {
    return this->statusCode;
}

void request::setStatusCode(int statusCode) {
	this->statusCode = statusCode;
}

string request::getQueryString() {
	return this->queryString;
}

std::string errorPageTamplate(std::string errorMessage)
{
    std::string filePath = "errorpage.html";
    std::ofstream outputFile(filePath, std::ios::trunc);

    if (outputFile.is_open())
    {
        outputFile << "<!DOCTYPE html>\n";
        outputFile << "<html>\n";
        	outputFile << "<head>\n";
        		outputFile << "<style>\n";
					outputFile << "html, body {font-family: 'Roboto Mono', monospace;font-size: 16px;}\n";
					outputFile << "body {background-color: black;margin: 0;padding: 0;}\n";
					outputFile << "p {color: white;font-size: 25px;letter-spacing: 0.2px;margin: 0;display: inline;}\n";
					outputFile << ".center-xy {text-align: center;top: 50%;left: 50%;transform: translate(-50%, -50%);position: absolute;}\n";
				outputFile << "</style>\n";
			outputFile << "</head>\n";

			outputFile << "<body>\n";
				outputFile << "<div class='center-xy'>\n";
					outputFile << "<p id='myP'>\n";
						outputFile << errorMessage + "\n";
					outputFile << "</p>\n";
				outputFile << "</div>\n";
				outputFile << "<script>\n";

				outputFile <<
						"let divElement = document.getElementById(\"myP\");"
						"let textContent = divElement.innerText.toString();"
						"let i = 1;"
						"function typeWriter() {"
						"	divElement.innerText = textContent.slice(0, i);"
						"	console.log(divElement.innerText);"
						"	i++;"
						"	if (i <= textContent.length)"
						"		setTimeout(typeWriter, 100);"
						"}"
						"setTimeout(typeWriter, 0);\n";
				outputFile << "</script>\n";
			outputFile << "</body>\n";
        outputFile << "</html>\n";



        // writeToFile(".container { width: 100%;}\n");


        

        // outputFile << "<div class='container'>\n";
        // outputFile << "<div class='copy-container center-xy'>\n";
        // outputFile << "<p>\n";
        // outputFile << errorMessage + "\n";

        // outputFile << "</div>\n";
        // writeToFile("</div>\n");

        outputFile.close();
        return filePath;
    }
    else
    {
        std::cout << "error" << std::endl;
        return "";
    }
}

std::string request::removeAndSetQueryString(const std::string& uri) {

	std::string::size_type queryStringPos = uri.find('?');

	if (queryStringPos != std::string::npos) {
		this->queryString = uri.substr(queryStringPos + 1);
		return uri.substr(0, queryStringPos);
	}
	this->queryString = "";
	return uri;
}

int request::checkRequestLine(std::string request)
{
	std::istringstream stream(request);
	std::string line;
	std::getline(stream, line);

	std::istringstream stream2(line);

	// std::cout << line << std::endl;

	stream2 >> this->method >> this->requestURI >> this->httpVersion;
	this->requestURI = removeAndSetQueryString(this->requestURI);
	if (this->method != "GET" && this->method != "POST" && this->method != "DELETE") {
		this->statusCode = 405;
		this->filePath = errorPageTamplate("405, Method Not Allowed.");
		return 1;
		printError("Method Not Allowed", 405);
	}
	if (this->requestURI.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos) {
		this->statusCode = 400;
		this->filePath = errorPageTamplate("400, Bad Request");
		return 1;
		printError("Bad Request", 400);
	}
	if (this->requestURI.size() > 2048)/* mazal request body larger than lbody li fl config file !!*/ {
		this->statusCode = 414;
		this->filePath = errorPageTamplate("414, Bad Request");
		return 1;
		printError("Bad Request", 414);
	}
	if (this->httpVersion != "HTTP/1.1") {
		this->statusCode = 505;
		this->filePath = errorPageTamplate("505, HTTP Version Not Supported");
		return 1;
		printError("HTTP Version Not Supported", 505);
	}
	return 0;
}

int request::checkHeaderFields(std::string headerFiles)
{
	// std::cout << request::method << std::endl;

	std::string line;
	std::vector<std::string> lines;
		
	std::istringstream iss(headerFiles);

	while (std::getline(iss, line)) {
		lines.push_back(line);
	}

	for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); i++) {
		if (i->find(":") != std::string::npos) {
			this->headerFields[i->substr(0, i->find(":"))] = i->substr(i->find(":") + 2);
		}
	}
	if (headerFields.find("Transfer-Encoding") != headerFields.end()
		&& headerFields["Transfer-Encoding"].find("chunked") == std::string::npos) {
		this->statusCode = 501;
		this->filePath = errorPageTamplate("501, Not implemented");
		return 1;
		printError("Not implemented", 501);
	}
	if (headerFields.find("Transfer-Encoding") == headerFields.end()
		&& headerFields.find("Content-Length") == headerFields.end()
		&& this->method == "POST") {
		this->statusCode = 400;
		this->filePath = errorPageTamplate("400, Bad Request");
		return 1;
		printError("Bad Request", 400);
	}
	// if (getMethod() == "GET") {
	// 	cout << "ra dart mn hna" << endl;
	// 	return 1;
	// }
	return 0;
}

void printChar(char c)
{
	if (c == '\n')
		std::cout << CYAN << "/n" << RESET_TEXT << std::endl;
	else if (c == '\r')
		std::cout << CYAN << "/r" << RESET_TEXT << std::endl;
	else
		std::cout << CYAN << c << RESET_TEXT << std::endl;
}

request::ParsingStatus request::parsChunked(char c)
{
	// cout << RED << "the char id: " << c << RESET_TEXT << endl;
	switch (currentChunkedState) {
	case Initial: {
		if (c == '\r') {
			currentChunkedState = request::ChunkDataCrLf;
		}
		else if (isxdigit(c)) {
			currentChunkedState = request::ParsingChunkSize;
		} 
		else {
			// error = "jdskjfkldjs";
			return request::ParsingFailed;
		}
	}
	case ParsingChunkSize: {
		if (isxdigit(c)) {
			chunkSize = (chunkSize * 16) + (isdigit(c) ? (c - '0') : (toupper(c) - 'A' + 10));
			// cout << "the c num is: " << c << " and chunksize is: " << chunkSize << endl;
			break;
		}
	}
	case AfterChunkSizeSpace: {
		if (c == ' ' || c == '\t')
			break;
		if (c == '\r') {
			currentChunkedState = request::ChunkSizeCarriageReturn;
			break;
		} else if (c == ';')
		{
			// parser.error = "faild chunked body";
			return request::ParsingFailed; // specific error should be returned
		}
		else
			return request::ParsingFailed;
	}
	case ChunkSizeCarriageReturn: {
		if (c == '\n') {
			currentChunkedState = request::ParsingChunkData;
			break;
		} else
		{
			// parser.error = "faild chunked body";
			return request::ParsingFailed;
		}
	}
	case ParsingChunkData: {
		if (chunkSize == 0) {
			return request::ParsingDone;
		}
		// cout << "ja hna  +++++++++++++++++++++" << chunkSize << endl;
		chunkSize--;
		std::ofstream outputFile("theBodyContentIsHere", std::ios::app);

			// cout << "ja hna +++++++++++++++++++++" << endl;
		if (outputFile.is_open() && this->method != "GET")
		{
			// cout << "dsjfhh" << endl;
			outputFile << c;
			outputFile.close();
		}
		else
		{
			// parser.error = "faild chunked body";
			return ParsingFailed;
		}
		if (chunkSize == 0) {
			currentChunkedState = request::ChunkDataCarriageReturn;
		}
		break;
	}
	case ChunkDataCarriageReturn: {
		if (c != '\r')
		{
			// parser.error = "faild chunked body";
			return request::ParsingFailed;
		}
		currentChunkedState = request::ChunkDataCrLf;
		break;
	}
	case ChunkDataCrLf: {
		if (c != '\n')
		{
			// parser.error = "faild chunked body";
			return request::ParsingFailed;
		}
		currentChunkedState = request::Initial;
		break;
	}
}
return request::ParsingContinue;
}

request::ParsingStatus request::checkBody(std::string body, server& _server)
{
	if ((int)body.size() > _server.getClientBodyLimit()) {
		this->statusCode = 413;
		this->filePath = errorPageTamplate("413, Request Entity Too Large");
		return ParsingFailed;
		printError("Request Entity Too Large", 413);
	}
	if (chunkSize == 0)
		currentChunkedState = Initial;
	else if (chunkSize > 0)
		currentChunkedState = ParsingChunkData;
	// cout << BLUE << "currentChunkedState is: " << currentChunkedState << " and chunk size is: "<< chunkSize <<"and c= " << RESET_TEXT << endl;
	for (size_t i = 0; i < body.size(); ++i)
	{
		char currentChar = body[i];
		ParsingStatus status = parsChunked(currentChar);
		switch (status)
		{
		case ParsingContinue:
			break;// Continue parsing
		case ParsingFailed:
			return ParsingFailed;
		case ParsingDone:
			currentChunkedState = Initial;// Reset state to Initial for the next chunk
			break;
		}
	}
	// cout << RED << "the parsing is done 2" << RESET_TEXT << endl;
	return ParsingDone;
}

request::ParsingStatus request::checkBody2(std::string body, server& _server)
{
	if ((int)body.size() > _server.getClientBodyLimit()) {
		this->statusCode = 413;
		this->filePath = errorPageTamplate("413, Request Entity Too Large");
		return ParsingFailed;
		printError("Request Entity Too Large", 413);
	}
	std::ofstream outputFile("theBodyContentIsHere", std::ios::app);
	if (outputFile.is_open() && this->method != "GET") {
		outputFile << body;
		outputFile.close();
	}
	bodyContentLength += body.size();
	return ParsingDone;
}

request::ParsingStatus request::checkBody3(std::string body, server& _server)
{
	if ((int)body.size() > _server.getClientBodyLimit()) {
		this->statusCode = 413;
		this->filePath = errorPageTamplate("413, Request Entity Too Large");
		return ParsingFailed;
		printError("Request Entity Too Large", 413);
	}
	// std::istringstream iss(string(&body[0], &body[0] + body.size()));
	std::istringstream iss(body);

	std::string line;
	std::string filename;

    while (getline(iss, line)) {
		if (line.find(boundary) != std::string::npos && flag == 0) {
			getline(iss, line);// skip boundary
			flag = 1;
		}
		// if (line.compare(boundary + "--") == 0) {
		// 		// end of file upload
		// 		break;
		// }
		if (line.find("Content-Disposition:") != std::string::npos)
		{
			bodyContentLength++;//? added to handel when there is no body
			gg = 1;
			if (line.find("filename=\"") != std::string::npos) {
				size_t filename_start = line.find("filename=\"") + 10;
				size_t filename_end = line.find('\"', filename_start);
				if (filename_start != std::string::npos && filename_end != std::string::npos) {
					filename = line.substr(filename_start, filename_end - filename_start);
				}
				getline(iss, line); // skip Content-Type line
				getline(iss, line); // skip empty line
			}
			else {
				filename = "form_field.txt";
				getline(iss, line); // skip empty line
			}
		}
		std::ofstream outputFile("filename", std::ios::app); //std::ios::binary);
		if (outputFile.is_open()) {
			cout << BLACK << line << RESET_TEXT << endl;
			if (gg == 0 || bodyContentLength == 1) {
				outputFile << line + '\n' ;//<< std::endl;
				// outputFile << std::endl;
				bodyContentLength += line.size() + 1;
			}
			while (getline(iss, line)) {
				gg = 0;
				if (line.find(boundary) != std::string::npos || line.find(boundary + "--") != std::string::npos) {
					flag = 0;
					break;
				}
				// if (line == "\r")
				// 	continue;
				cout << BLACK << line << RESET_TEXT << endl;
				if (iss.tellg() != -1)
					outputFile << line + '\n' ;//<< std::endl;
				else
					outputFile << line;
				bodyContentLength += line.size() + 1;
			}
			outputFile.close();
			if (line.find(boundary + "--") != std::string::npos) {
				bodyContentLength = this->actualContentLength;
			}
		}
	}
	return ParsingDone;
}

int request::getBytesRange()
{
	return bytesRange;
}

void request::setBytesRange()
{
	std::string input = this->headerFields["Range"];
	std::string result;

	for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
		if (std::isdigit(*it)) {
			result += *it;
		}
	}
	std::istringstream(result) >> this->bytesRange;
}

void request::setContentLength()
{
	std::string input = this->headerFields["Content-Length"];
	std::string result;

	for (std::string::iterator it = input.begin(); it != input.end(); ++it) {
		if (std::isdigit(*it)) {
			result += *it;
		}
	}
	std::istringstream(result) >> this->actualContentLength;
}

void generatePrefixes(const std::string& path, std::vector<std::string>& prefixes) {
	std::istringstream iss(path);
	std::string component;
	std::string currentPrefix = "/";  // Start with the root "/"

	while (std::getline(iss, component, '/')) {
		if (!component.empty()) {
			currentPrefix += component + "/";  // Add the component to the current prefix
			prefixes.push_back(currentPrefix);
		}
	}
}

bool isDirectory(const char* path)
{
	struct stat fileInfo;

	if (stat(path, &fileInfo) != 0) {
		return false;
	}
	return S_ISDIR(fileInfo.st_mode);// need to check if this is allowed
}

bool fileExists(const char* path) {
	struct stat fileInfo;

	if (path[0] == '/')
		path = path + 1;
	if (stat(path, &fileInfo) != 0) {
		return false;
	}
	return S_ISREG(fileInfo.st_mode);
}

int request::matchLocation(server& _server)
{
	std::vector<Location> vec;
	vec = _server.getLocations();
	std::string paths = this->requestURI;

	// std::cout << MAGENTA << "***** the path *****" << paths << RESET_TEXT << std::endl;

	if (fileExists(paths.c_str()))
	{
		std::cout << GREEN << "***** flbla *****" << RESET_TEXT << std::endl;
		filePath = this->requestURI;
		if (filePath[0] == '/')
			filePath = filePath.substr(1);
		return 0;
	}

	while (!paths.empty())
	{
		// std::cout << GREEN << "***** looping *****" << RESET_TEXT << std::endl;
		for (std::vector<Location>::iterator it = vec.begin(); it != vec.end(); it++) {
			// std::cout << RED << "is LOCATION: " << it->getLocationName() << "\t\tequal to URI: " << paths << RESET_TEXT << std::endl;

			if (it->getLocationName() == paths) {
				std::cout << GREEN << "*****FOUND A MATCH*****" << RESET_TEXT << std::endl;
				filePath = it->getRoot() + this->requestURI;
				if (isDirectory(filePath.c_str())) {
				std::cout << GREEN << "*****FOUND A MATCH 22 *****" << RESET_TEXT << std::endl;
					filePath = filePath + it->getIndex();
				}
				loc = *it;
				// cout << BLUE <<loc.getCgiPath()<< RESET_TEXT << endl;
				if (filePath[0] == '/')
					filePath = filePath.substr(1);
				return 0;
			}
		}
		std::size_t slashLoc = paths.find_last_of('/');
		if (slashLoc != std::string::npos) {
			size_t n = std::count(paths.begin(), paths.end(), '/');
			if (n > 1)
				paths = paths.substr(0, slashLoc);
			else
				paths = "/";
		}
	}
	return 1;
}

int request::parseRequest(std::string request, server& _server)
{
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++99"<< RESET_TEXT << endl;
		// cout << BLUE << "wajadnah ..............................." << RESET_TEXT << endl;
	std::cout << WHITE << request << RESET_TEXT << std::endl;
	std::cout << MAGENTA << currentChunkedState << " &&& "<< bodyContentLength << RESET_TEXT << std::endl;
	if (currentChunkedState != ParsingChunkData && bodyContentLength == 0) {
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++ taking request line 990001"<< RESET_TEXT << endl;
		if (checkRequestLine(request))
			return 1;
	}

	if (currentChunkedState != ParsingChunkData && bodyContentLength == 0) {
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++ taking header fields 990002"<< RESET_TEXT << endl;
		if (checkHeaderFields(request.substr(0, request.find("\r\n\r\n")))){
			return 1;
		}
	}
	// cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++ ou 99000"<< RESET_TEXT << endl;
	setContentLength();//!
	setContentType();
	// if (matchLocation(_server)) {
	// std::cout << MAGENTA << "NO location matched" << RESET_TEXT << std::endl;
	// }
	/*remove the / from the begining of the path*/
	// if (filePath[0] == '/')
	// 	filePath = filePath.substr(1);
	// if (!fileExists(filePath.c_str()))
	// {
	// 	this->statusCode = 404;
	// 	this->filePath = errorPageTamplate("404, Not Found.");
	// 	return 1;
	// 	printError("Not Found.", 404);
	// }
	if (getMethod() == "GET")
		return 1;
	std::ofstream outputFile("sheeeeeeeeeee", std::ios::app); //std::ios::binary);
		if (outputFile.is_open()) {
			outputFile << request;
		}
		// cout << BLUE << "wajadnah ..............................."<< bodyContentLength << RESET_TEXT << endl;
	if (bodyContentLength > 0 && bodyContentLength < this->actualContentLength)
	{
		cout << BLUE << "wajadnah ............................... in content length body condition _ _________________"<< RESET_TEXT << endl;
		if (headerFields.find("Content-Type") != headerFields.end()
			&& headerFields["Content-Type"].find("multipart/form-data") != std::string::npos) {
			checkBody3(request, _server);
			cout << BLUE << "wajadnah ............................... in content length body |boundary after it get headers| condition _ _________________"<< RESET_TEXT << endl;

		}
		else {

			cout << BLUE << "wajadnah ............................... in content length body |No boundary after getting headers| condition _ _________________"<< RESET_TEXT << endl;
			checkBody2(request, _server);
		}
	}
	else if (currentChunkedState == 4) {

		cout << BLUE << "wajadnah ..............................   in chunked body after getting headers   .0"<< RESET_TEXT << endl;
		checkBody(request, _server);
	}
	else {
		
		if (headerFields.find("Transfer-Encoding") != headerFields.end()) {//!add chunked to this condition
			cout << BLUE << "wajadnah ..........................    chunked with headers  ....1"<< RESET_TEXT << endl;
			checkBody(request.substr(request.find("\r\n\r\n") + 4), _server);
		}
		else {
			if (headerFields.find("Content-Type") != headerFields.end()
			&& headerFields["Content-Type"].find("multipart/form-data") != std::string::npos) {
				cout << BLUE << "wajadnah ..........................     boundary with headers    .....2"<< RESET_TEXT << endl;
				checkBody3(request.substr(request.find("\r\n\r\n") + 4), _server);
			}
			else {
				checkBody2(request.substr(request.find("\r\n\r\n") + 4), _server);
				cout << BLUE << "wajadnah ........................   content length with headers   .......3"<< RESET_TEXT << endl;
			}
		}
	}

	// cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++"<< RESET_TEXT << endl;
	if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") == headerFields.end()
		&& chunkSize != 0) {
			requestStatus = false;
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++ 1"<< RESET_TEXT << endl;
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") == std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& bodyContentLength < this->actualContentLength) {
			if (bodyContentLength < this->actualContentLength)
			requestStatus = false;
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++2"<< RESET_TEXT << endl;
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& chunkSize != 0) {
			requestStatus = false;
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++3"<< RESET_TEXT << endl;
			return 0;
		}
	else {
	cout << BLUE << "KIDOZ MN HNA _++++++++++++++++++++++++++4"<< RESET_TEXT << endl;
		cout <<MAGENTA << "HNA AAAAAAA@#$%^&" << RESET_TEXT <<endl;
		return 1;
	}
}

void request::setContentType()
{
	addAllContentTypes();
	if (this->method == "GET") {
		if (isDirectory(requestURI.c_str())) {
			this->ContentType = "text/html";
		}
		else {
			std::string fileExtension;
			size_t dotPosition = requestURI.rfind(".");

			if (dotPosition != std::string::npos) {
				fileExtension = requestURI.substr(dotPosition);
			}
			else {
				std::cerr << "Error: No dot found in requestURI\n";
			}
			this->ContentType = allContTypes[fileExtension];
		}
	}
	if (this->method == "POST") {
		this->ContentType = this->headerFields["Content-Type"];
		this->boundary = "--" + this->ContentType.substr(this->ContentType.find("boundary=") + 9);
		size_t lastNonSpace = this->boundary.find_last_not_of(" \t\r\n");

		if (lastNonSpace != std::string::npos) {
			this->boundary.erase(lastNonSpace + 1);
		}
		// cout << BLUE<< ">>>>>" <<this->boundary.size() << "|" << this->boundary << "|" << RESET_TEXT << endl;
		// if (this->boundary[this->boundary.size() - 1] == '\r')
		// 	this->boundary = this->boundary.substr(0, this->boundary.size() - 1);
		// cout << BLUE<< ">>>>>" <<this->boundary.size() << "|" << this->boundary << "|" << RESET_TEXT << endl;
	}
}

void	request::addAllContentTypes()
{
	allContTypes[".aac"] = "audio/aac";
	allContTypes[".abw"] = "application/x-abiword";
	allContTypes[".arc"] = "application/x-freearc";
	allContTypes[".avif"] = "image/avif";
	allContTypes[".avi"] = "video/x-msvideo";
	allContTypes[".azw"] = "application/vnd.amazon.ebook";
	allContTypes[".bin"] = "application/octet-stream";
	allContTypes[".bmp"] = "image/bmp";
	allContTypes[".bz"] = "application/x-bzip";
	allContTypes[".bz2"] = "application/x-bzip2";
	allContTypes[".cda"] = "application/x-cdf";
	allContTypes[".csh"] = "application/x-csh";
	allContTypes[".css"] = "text/css";
	allContTypes[".csv"] = "text/csv";
	allContTypes[".doc"] = "application/msword";
	allContTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	allContTypes[".eot"] = "application/vnd.ms-fontobject";
	allContTypes[".epub"] = "application/epub+zip";
	allContTypes[".gz"] = "application/gzip";
	allContTypes[".gif"] = "image/gif";
	allContTypes[".htm"] = "text/html";
	allContTypes[".html"] = "text/html";
	allContTypes[".ico"] = "image/vnd.microsoft.icon";
	allContTypes[".ics"] = "text/calendar";
	allContTypes[".jar"] = "application/java-archive";
	allContTypes[".jpeg"] = "image/jpeg";
	allContTypes[".jpg"] = "image/jpeg";
	allContTypes[".js"] = "text/javascript";
	allContTypes[".json"] = "application/json";
	allContTypes[".jsonld"] = "application/ld+json";
	allContTypes[".mid"] = "audio/midi";
	allContTypes[".midi"] = "audio/midi";
	allContTypes[".mjs"] = "text/javascript";
	allContTypes[".mp3"] = "audio/mpeg";
	allContTypes[".mp4"] = "video/mp4";
	allContTypes[".mpeg"] = "video/mpeg";
	allContTypes[".mpkg"] = "application/vnd.apple.installer+xml";
	allContTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
	allContTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	allContTypes[".odt"] = "application/vnd.oasis.opendocument.text";
	allContTypes[".oga"] = "audio/ogg";
	allContTypes[".ogv"] = "video/ogg";
	allContTypes[".ogx"] = "application/ogg";
	allContTypes[".opus"] = "audio/opus";
	allContTypes[".otf"] = "font/otf";
	allContTypes[".png"] = "image/png";
	allContTypes[".pdf"] = "application/pdf";
	allContTypes[".php"] = "application/x-httpd-php";
	allContTypes[".ppt"] = "application/vnd.ms-powerpoint";
	allContTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	allContTypes[".rar"] = "application/vnd.rar";
	allContTypes[".rtf"] = "application/rtf";
	allContTypes[".sh"] = "application/x-sh";
	allContTypes[".svg"] = "image/svg+xml";
	allContTypes[".tar"] = "application/x-tar";
	allContTypes[".tif"] = "image/tiff";
	allContTypes[".tiff"] = "image/tiff";
	allContTypes[".ts"] = "video/mp2t";
	allContTypes[".ttf"] = "font/ttf";
	allContTypes[".txt"] = "text/plain";
	allContTypes[".vsd"] = "application/vnd.visio";
	allContTypes[".wav"] = "audio/wav";
	allContTypes[".weba"] = "audio/webm";
	allContTypes[".webm"] = "video/webm";
	allContTypes[".webp"] = "image/webp";
	allContTypes[".woff"] = "font/woff";
	allContTypes[".woff2"] = "font/woff2";
	allContTypes[".xhtml"] = "application/xhtml+xml";
	allContTypes[".xls"] = "application/vnd.ms-excel";
	allContTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	allContTypes[".xml"] = "application/xml";
	allContTypes[".xul"] = "application/vnd.mozilla.xul+xml";
	allContTypes[".zip"] = "application/zip";
	allContTypes[".3gp"] = "video/3gpp"; // audio/3gpp
	allContTypes[".3g2"] = "video/3gpp2"; // audio/3gpp2
	allContTypes[".7z"] = "application/x-7z-compressed";
}
