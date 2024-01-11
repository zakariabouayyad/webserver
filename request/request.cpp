#include "../server.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

request::request() : requestStatus(true) {
	this->chunkSize = 0;
	this->bodyContentLength = 0;

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

bool	request::isRequestDone() {
	return requestStatus;
}

void request::checkRequestLine(std::string request)
{
	std::istringstream stream(request);
	std::string line;
	std::getline(stream, line);

	std::istringstream stream2(line);

	// std::cout << line << std::endl;

	stream2 >> this->method >> this->requestURI >> this->httpVersion;
	// if (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
	//	 printError("Method Not Allowed", 405);
	// if (this->requestURI.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos)
	//	 printError("Bad Request", 400);
	// if (this->requestURI.size() > 2048)// mazal request body larger than lbody li fl config file !!
	//	 printError("Request-URI Too Long", 414);
	// if (this->httpVersion != "HTTP/1.1")
	//	 printError("HTTP Version Not Supported", 505);
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
			this->headerFields[i->substr(0, i->find(":"))] = i->substr(i->find(":") + 1);
		}
	}
	if (headerFields.find("Transfer-Encoding") != headerFields.end()
		&& headerFields["Transfer-Encoding"].find("chunked") == std::string::npos)
		printError("Not implemented", 501);
	if (headerFields.find("Transfer-Encoding") == headerFields.end()
		&& headerFields.find("Content-Length") == headerFields.end()
		&& this->method == "POST")
		printError("Bad Request", 400);
	if (getMethod() == "GET") {
		cout << "ra dart mn hna" << endl;
		return 1;
	}
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
	if ((int)body.size() > _server.getClientBodyLimit())
		printError("Request Entity Too Large", 413);
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
			// cout << RED << "the parsing continue" << RESET_TEXT << endl;
			break;// Continue parsing
		case ParsingFailed:
			// cout << RED << "the parsing failed" << RESET_TEXT << endl;
			return ParsingFailed;
		case ParsingDone:
			cout << RED << "the parsing is done 1" << RESET_TEXT << endl;
			currentChunkedState = Initial;// Reset state to Initial for the next chunk
			break;
		}
	}
	cout << RED << "the parsing is done 2" << RESET_TEXT << endl;
	return ParsingDone;
}

request::ParsingStatus request::checkBody2(std::string body, server& _server)
{
	cout << RED << "ENTRED check body 2" << RESET_TEXT << endl;
	if ((int)body.size() > _server.getClientBodyLimit())
		printError("Request Entity Too Large", 413);
	// if (chunkSize == 0)
	// 	currentChunkedState = Initial;
	// else if (chunkSize > 0)
	// 	currentChunkedState = ParsingChunkData;
	// cout << BLUE << "currentChunkedState is: " << currentChunkedState << " and chunk size is: "<< chunkSize <<"and c= "<< body[10] << RESET_TEXT << endl;
	size_t i;
	
	cout << BLUE << "size = " << body.size() << RESET_TEXT << endl;
	cout << WHITE << "body = " << body << RESET_TEXT << endl;
	for (i = 0; i < body.size(); ++i)
	{
		char currentChar = body[i];
		std::ofstream outputFile("theBodyContentIsHere", std::ios::app);
		if (outputFile.is_open() && this->method != "GET")
		{
			// cout << "dsjfhh" << endl;
			outputFile << currentChar;
			outputFile.close();
		}
		// ParsingStatus status = parsChunked(currentChar);
		// switch (status)
		// {
		// case ParsingContinue:
		// 	// cout << RED << "the parsing continue" << RESET_TEXT << endl;
		// 	break;// Continue parsing
		// case ParsingFailed:
		// 	// cout << RED << "the parsing failed" << RESET_TEXT << endl;
		// 	return ParsingFailed;
		// case ParsingDone:
		// 	cout << RED << "the parsing is done 1" << RESET_TEXT << endl;
		// 	currentChunkedState = Initial;// Reset state to Initial for the next chunk
		// 	break;
		// }
	}
	cout << BLUE << "i = " << i << RESET_TEXT << endl;
	bodyContentLength += i;
	cout << RED << "the parsing is done 3   " << "bodyContentLength is: " << bodyContentLength << RESET_TEXT << endl;
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
	// std::cout << "bytes tedtts  ; " << this->headerFields["Range"] << std::endl;
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
	// std::cout << "bytes tedtts  ; " << this->headerFields["Range"] << std::endl;
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
		// std::cout << GREEN << "***** flbla *****" << RESET_TEXT << std::endl;
		filePath = this->requestURI;
		return 0;
	}

	while (!paths.empty())
	{
		// std::cout << GREEN << "***** looping *****" << RESET_TEXT << std::endl;
		for (std::vector<Location>::iterator it = vec.begin(); it != vec.end(); it++) {
			// std::cout << RED << "is LOCATION: " << it->getLocationName() << "\t\tequal to URI: " << paths << RESET_TEXT << std::endl;
			if (it->getLocationName() == paths) {
				// std::cout << GREEN << "*****FOUND A MATCH*****" << RESET_TEXT << std::endl;
				filePath = it->getRoot() + this->requestURI;
				if (isDirectory(filePath.c_str()))
				{
					// cout << RED << "___ it's a directory ___" << RESET_TEXT << endl;
					filePath = filePath + it->getIndex();
				}
				// std::cout << BLUE << filePath << RESET_TEXT << std::endl;
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
	// std::cout << WHITE << request << RESET_TEXT << std::endl;
	std::ofstream output("theultimattest", std::ios::app);
	output << request;
	output << "\n++++++++++++++++++++++++++++++++++++++++++\n";
		// cout << RED << "||||||||||||||||||||" << bodyContentLength << RESET_TEXT << endl;
	if (currentChunkedState != ParsingChunkData && bodyContentLength == 0) {
		// cout << RED << "||||||||||||||||||||" << bodyContentLength << RESET_TEXT << endl;
		checkRequestLine(request);
	}
		
	if (currentChunkedState != ParsingChunkData && bodyContentLength == 0)
		checkHeaderFields(request.substr(0, request.find("\r\n\r\n")));
	// if (checkHeaderFields(request.substr(0, request.find("\r\n\r\n"))))
	// {
	setContentLength();//!
	setContentType();
	if (matchLocation(_server)) {
	std::cout << MAGENTA << "NO location matched" << RESET_TEXT << std::endl;
	}
	/*remove the / from the begining of the path*/
	if (filePath[0] == '/')
	filePath = filePath.substr(1);
	if (getMethod() == "GET")
		return 1;

	// setContentLength();//!
	// cout << bodyContentLength<< "&& "<< this->actualContentLength << endl;
	if (bodyContentLength > 0 && bodyContentLength < this->actualContentLength)
	{
		// cout << "DEEEZ NTS ________________" << endl;
		checkBody2(request, _server);
	}
	else if (currentChunkedState == 4)
		checkBody(request, _server);
	else {
		if (headerFields.find("Transfer-Encoding") != headerFields.end()) {
			// cout << "first body" << endl;
			checkBody(request.substr(request.find("\r\n\r\n") + 4), _server);
		}
		else {
			// cout << "seconde body" << endl;
			checkBody2(request.substr(request.find("\r\n\r\n") + 4), _server);
		}
	}

	cout << MAGENTA << chunkSize << RESET_TEXT << endl;

	// if (bodyContentLength >= this->actualContentLength)
	// {
	// 	cout << chunkSize << endl;
	// 	cout << bodyContentLength << endl;
	// 	cout << this->actualContentLength << endl;
	// 	setContentType();//! need to remove comment after chuncked body done
	// }

	// if (matchLocation(_server)) {
	// 	std::cout << MAGENTA << "NO location matched" << RESET_TEXT << std::endl;
	// }

	// std::cout << BLUE << "---> " << this->filePath << RESET_TEXT << std::endl;
	/*remove the / from the begining of the path*/
	// if (filePath[0] == '/')
	// 	filePath = filePath.substr(1);
	// cout << BLUE << " and chunk size is: ============================================================ "<< chunkSize << RESET_TEXT << endl;
	// if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos && chunkSize != 0 || bodyContentLength < this->actualContentLength) {
	// if (chunkSize != 0 || (bodyContentLength < this->actualContentLength && headerFields["Transfer-Encoding"].find("chunked") == std::string::npos)) {
	// 	cout << "D5l hna ++++++++++++++++++++++" << endl;
	// 	requestStatus = false;
	// 	return 0;
	// }

	if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") == headerFields.end()
		&& chunkSize != 0) {
			cout << RED << "===============1" << RESET_TEXT << endl;
			requestStatus = false;
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") == std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& bodyContentLength < this->actualContentLength) {
			if (bodyContentLength < this->actualContentLength)
				cout << RED << "AAHHHH" << RESET_TEXT << endl;
			cout << RED << "===============2" << RESET_TEXT << endl;
			requestStatus = false;
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& chunkSize != 0) {
			cout << RED << "===============3" << RESET_TEXT << endl;
			requestStatus = false;
			return 0;
		}
	else{
		cout << RED << "===============4" << RESET_TEXT << endl;
		return 1;
	}
	// if (headerFields.find("Content-Length") != headerFields.end() && bodyContentLength < this->actualContentLength) {
	// 	cout << "D5l hna =======================" << bodyContentLength << ">>>> " << this->actualContentLength << endl;
	// 	requestStatus = false;
	// 	return 0;
	// }
}

void request::setContentType()
{
	addAllContentTypes();
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