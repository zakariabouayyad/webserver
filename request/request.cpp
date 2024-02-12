#include "../server.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

request::request() {
	this->chunkSize = 0;
	this->bodyContentLength = 0;
	this->bodyDone = 0;
	this->headersDone = starting;
	this->flag = 0;
	this->gg = 0;
	this->is_CGI = 0;
	this->redirectURL = "";
	this->statusCode = "200 OK";
	this->failHeader = false;
	this->keepAlive = false;
	this->Cgisdone = 0;
	this->gotRequestLine = false;
}

void	request::reset() {
	// this->requestStatus = true;
	this->chunkSize = 0;
	this->bodyContentLength = 0;
	this->bodyDone = 0;
	this->headersDone = starting;
	this->flag = 0;
	this->gg = 0;
	this->is_CGI = 0;
	this->redirectURL = "";
	this->statusCode = "200 OK";
	this->failHeader = false;
	this->keepAlive = false;

	this->method = "";
	this->requestURI = "";
	this->httpVersion = "";
	this->body = "";
	this->ContentType = "";
	this->uploadContentType = "";
	this->filePath = "";
	this->redirectURL = "";
	this->queryString = "";
	this->boundary = "";
	this->filename = "";
	this->cgi_header = "";
	this->cgi_body = "";
	this->headers = "";
	this->theBody = "";
	this->gotRequestLine = false;

	allContTypes.clear();
	headerFields.clear();
}

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

std::string request::getMethod() {
	return this->method;
}

string request::getrequestURI() {
	return this->requestURI;
}

string request::getredirectURL() {
	return this->redirectURL;
}

string request::getContentType() {
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

// bool	request::isRequestDone() {
// 	return requestStatus;
// }

std::string request::getStatusCode() {
    return this->statusCode;
}

void request::setStatusCode(std::string statusCode) {
	this->statusCode = statusCode;
}

string request::getQueryString() {
	return this->queryString;
}

void request::setredirectURL(string redir)
{
	this->redirectURL = redir;
}

std::string	request::getUploadPath()
{
	return this->uploadPath;
}

void	request::setUploadPath(std::string path)
{
	this->uploadPath = path;
}

std::string errorPageTamplate(std::string errorMessage)
{
	std::string filePath = "errorpage.html";
	if (fileExists(filePath.c_str()))
		unlink(filePath.c_str());
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
	
		outputFile.close();
	}
	if (outputFile.fail())
	{
		std::cout << "error" << std::endl;
		std::string filePath = "/Users/zbouayya/goinfre/errorpage.html";
		if (fileExists(filePath.c_str()))
			unlink(filePath.c_str());
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

			outputFile.close();
		}
	}
	return filePath;
}

std::string removewhites(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");

	if (start == std::string::npos || end == std::string::npos) {
		return "";  // String is all whitespaces
	}
	return str.substr(start, end - start + 1);
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

void request::setErrorPage(std::string statusCode, std::string errorMsg)
{
	int key = extractStatusCode(statusCode);

	this->statusCode = statusCode;
	this->ContentType = "text/html";

	if (errorpages.find(key) != errorpages.end()) {

		if (access(errorpages[key].c_str(), R_OK) == 0) {
			setFilePath(errorpages[key]);
		}
		else {
			this->filePath = errorPageTamplate(errorMsg);
		}
	}
	else {
		this->filePath = errorPageTamplate(errorMsg);
	}
}

int charToHexadecimal(char ch) {
	if (isdigit(ch))
		return ch - '0';
	else if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	else if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	else
		return -1; // Invalid hex character
}

std::string	URIEncoding(std::string	&path)
{
	std::string res;
	for (int i = 0; i < (int)path.length(); ++i) {
		if (path[i] == '+') {
			res += ' ';
			i++;
		}
		if (path[i] == '%') {
			if (i + 2 < (int)path.length() && isxdigit(path[i + 1]) && isxdigit(path[i + 2])) {
				char high = charToHexadecimal(path[i + 1]);
				char low = charToHexadecimal(path[i + 2]);
				if (high != -1 && low != -1) {
					res += high * 16 + low;
					i += 2;
				} else {
					res += path[i];
				}
			} else {
				res += path[i];
			}
		} else {
			res += path[i];
		}
	}
	return res;
}


int request::checkRequestLine(std::string request, int state)
{
	if (gotRequestLine == false) {
		std::istringstream stream(request);
		std::string line;
		std::getline(stream, line);

		std::istringstream stream2(line);

		if (!this->method.empty() || !this->requestURI.empty() || !this->httpVersion.empty()) {
			return 0;
		}
		stream2 >> this->method >> this->requestURI >> this->httpVersion;
		gotRequestLine = true;
		// this->requestURI = URIEncoding(this->requestURI);
	}

	if (state == headerFieldState || state == headersDoneState) {
		if (this->method.empty() || this->requestURI.empty() || this->httpVersion.empty() ) {
			// cout << "HERE1" << endl;
			setErrorPage("400 Bad Request", "400, Bad Request");
			return 1;
		}
	}

	this->requestURI = removeAndSetQueryString(this->requestURI);
	if (this->method != "GET" && this->method != "POST" && this->method != "DELETE") {
		setErrorPage("405 Method Not Allowed", "405, Method Not Allowed.");
		return 1;
	}
	if (this->requestURI.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%") != std::string::npos) {
			// cout << "HERE2" << this->requestURI << endl;

		setErrorPage("400 Bad Request", "400, Bad Request");
		return 1;
	}
	if (this->requestURI.size() + this->queryString.size() > 2048)/* mazal request body larger than lbody li fl config file !!*/ {
		// cout << RED << "HERE" << RESET_TEXT << endl;
		setErrorPage("414 URI Too Long", "414, URI Too Long.");
		return 1;
	}
	if (this->httpVersion != "HTTP/1.1") {
		setErrorPage("505 HTTP Version Not Supported", "505, HTTP Version Not Supported");
		return 1;
	}
	this->requestURI = URIEncoding(this->requestURI);
	return 0;
}

int request::checkHeaderFields(std::string headerFiles, int state)
{
	std::string line;
	std::vector<std::string> lines;
		
	std::istringstream iss(headerFiles);

	while (std::getline(iss, line)) {
		lines.push_back(line);
	}

	for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); i++) {
		if (i->find(":") != std::string::npos) {
			this->headerFields[removewhites(i->substr(0, i->find(":")))] = removewhites(i->substr(i->find(":") + 2));
		}
	}
	if (headerFields.find("Host") == headerFields.end() && state == headersDoneState) {
		setErrorPage("400 Bad Request", "400, Bad Request");
		return 2;
	}
	if (headerFields.find("Transfer-Encoding") == headerFields.end()
		&& headerFields.find("Content-Length") == headerFields.end()
		&& this->method == "POST") {
		setErrorPage("400 Bad Request", "400, Bad Request");
		return 1;
	}
	if (headerFields.find("Transfer-Encoding") != headerFields.end()
		&& headerFields["Transfer-Encoding"] != "chunked") {
		setErrorPage("501 Not implemented", "501, Not implemented");
		return 1;
	}
	return 0;
}

std::string getTheExtensionFromContentType(std::string daContentType)
{
	// Find the position of the '/'
	size_t slashPos = daContentType.find('/');

	if (slashPos != std::string::npos) {
		// Extract the substring after the '/'
		std::string fileType = daContentType.substr(slashPos + 1);

		// Replace any '+' characters with '_' (e.g., image/jpeg -> jpeg)//!
		size_t plusPos = fileType.find('+');
		while (plusPos != std::string::npos) {
			fileType[plusPos] = '_';
			plusPos = fileType.find('+', plusPos + 1);
		}
		// Return the file extension
		return "." + fileType;
	} else {
		// Return an empty string if no '/' is found
		return "";
	}
}

std::string generateRandomFileName() {
	std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int length = 8;

	std::string randomFileName;
	srand(static_cast<unsigned>(time(0)));

	for (int i = 0; i < length; ++i) {
		int randomIndex = rand() % characters.length();
		randomFileName += characters[randomIndex];
	}

	return randomFileName;
}

int hexCharToInt(char hexChar) {
	if (isdigit(hexChar))
		return hexChar - '0';
	else if (islower(hexChar))
		return hexChar - 'a' + 10;
	else if (isupper(hexChar))
		return hexChar - 'A' + 10;
	else
		return -1; // Invalid hex character
}

unsigned int hexToDecimal(const std::string& hexString) {
	unsigned int decimalValue = 0;
	for (size_t i = 0; i < hexString.length(); ++i) {
		int digitValue = hexCharToInt(hexString[i]);
		if (digitValue == -1) {
			// Invalid hex character
			// You might want to handle this case differently depending on your requirements
			std::cerr << "Invalid hex character: " << hexString[i] << std::endl;
			return 0; // Return 0 in case of error
		}
		decimalValue = (decimalValue << 4) + digitValue;
	}
	return decimalValue;
}

request::ParsingStatus request::parseChunked(std::string body, server& _server)
{
	std::string fileName;
	if ((int)body.size() > _server.getClientBodyLimit()) {
		setErrorPage("413 Request Entity Too Large", "413, Request Entity Too Large");
		return ParsingFailed;
	}
	std::istringstream stream(body);
	while (true) {
		std::string chunkSizeHex;
		std::getline(stream, chunkSizeHex);

		// Convert hex string to integer
		// std::istringstream iss(chunkSizeHex);
		std::size_t chunkSize;
		chunkSize = hexToDecimal(removewhites(chunkSizeHex));
		// iss >> std::hex >> chunkSize;

		// if (iss.fail()) {
		// 	setErrorPage("500 Internal Server Error", "500, Internal Server Error.");
		// 	return ParsingFailed;
		// }
		// Check for the end of chunks
		if (chunkSize == 0) {
			break;
		}

		// Read the chunk data
		std::string chunkData(chunkSize, '\0');
		stream.read(&chunkData[0], chunkSize);

		// Consume the CRLF at the end of the chunk
		std::string crlf;
		std::getline(stream, crlf);
		fileName = generateRandomFileName() + getTheExtensionFromContentType(this->uploadContentType);
		std::ofstream outputFile("upload/" + fileName, std::ios::app);
		if(!outputFile.is_open())
		{
			setErrorPage("500 Internal Server Error", "500, Internal Server Error.");
			return ParsingFailed;
		}
		if(outputFile.is_open())
		{
			setUploadPath("upload/" + fileName);
			outputFile << chunkData;
			if (outputFile.fail()) {
				outputFile.close();
				setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
				return ParsingFailed;
			}
			outputFile.close();
		}
		if (outputFile.fail()) {
			outputFile.close();
			setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
			return ParsingFailed;
		}
		outputFile.close();
	}
	return ParsingDone;
}

request::ParsingStatus request::parseContentLength(std::string body, server& _server)
{
	if ((int)body.size() > _server.getClientBodyLimit()) {
		setErrorPage("413 Request Entity Too Large", "413, Request Entity Too Large");
		return ParsingFailed;
	}
	std::string fileName;
	fileName = generateRandomFileName() + getTheExtensionFromContentType(this->uploadContentType);
	std::ofstream outputFile("upload/" + fileName, std::ios::app);
	if(!outputFile.is_open())
	{
		setErrorPage("500 Internal Server Error", "500, Internal Server Error.");
		return ParsingFailed;
	}
	if (outputFile.is_open()) {
		setUploadPath("upload/" + fileName);
		outputFile << body;
		if (outputFile.fail()) {
			outputFile.close();
			setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
			return ParsingFailed;
		}
	}
	if (outputFile.fail()) {
		outputFile.close();
		setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
		return ParsingFailed;
	}
	bodyContentLength += body.size();
	outputFile.close();
	return ParsingDone;
}

std::string extractFilename(const std::string& boundaryHeaders) {
	std::string filename;
	size_t filenamePos = boundaryHeaders.find("filename=\"");
	
	if (filenamePos != std::string::npos) {
		filenamePos += 10;  // Move to the start of the filename
		size_t endPos = boundaryHeaders.find("\"", filenamePos);

		if (endPos != std::string::npos) {
			filename = boundaryHeaders.substr(filenamePos, endPos - filenamePos);
		}
	}
	else
		filename = "newname";
	return filename;
}

bool fileExists2(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    return file.is_open();
}

std::string generateNewFileName(const std::string& fileName) {
	size_t dotPosition = fileName.find_last_of('.');
	std::string baseName;
	std::string extension;
	if (dotPosition != std::string::npos) {
		baseName = fileName.substr(0, dotPosition);
		extension = fileName.substr(dotPosition);
	}
	else {
		baseName = fileName;
		extension = "";
	}

	int counter = 2;
	std::string newFileName;
	newFileName = fileName;
	bool exist = fileExists2("upload/" + fileName);
	while (exist)
	{
		std::ostringstream oss;
		oss << baseName << counter << extension;
		newFileName = oss.str();
		exist = fileExists2("upload/" + newFileName);
		counter++;
	}

	return newFileName;
}

request::ParsingStatus request::parseBoundary(std::string body, server& _server)
{
	if ((int)body.size() > _server.getClientBodyLimit()) {
		setErrorPage("413 Request Entity Too Large", "413, Request Entity Too Large");
		return ParsingFailed;
	}
	size_t pos = 0;
	size_t nextPos = 0;
	while ((nextPos = body.find(boundary, pos)) != std::string::npos)
	{
		std::string part = body.substr(pos, nextPos - pos);
		if (part.find("\r\n\r\n") != std::string::npos)
		{
			std::string fileName = extractFilename(part.substr(0, part.find("\r\n\r\n")));
			fileName = generateNewFileName(fileName);
			std::ofstream outputFile("upload/" + fileName);
			if(!outputFile.is_open())
			{
				setErrorPage("500 Internal Server Error", "500, Internal Server Error.");
				return ParsingFailed;
			}
			if (outputFile.is_open())
			{
				setUploadPath("upload/" + fileName);
				std::string bodySent = part.substr(part.find("\r\n\r\n") + 4);
				if (bodySent.empty() || bodySent == "\r\n") {
					unlink(("upload/" + fileName).c_str());
				}
				outputFile << bodySent.substr(0, bodySent.size() - 2);
				if (outputFile.fail()) {
					outputFile.close();
					setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
					return ParsingFailed;
				}
				outputFile.close();
				if (outputFile.fail()) {
					outputFile.close();
					setErrorPage("507 Insufficient Storage", "507, Insufficient Storage.");
					return ParsingFailed;
				}
				outputFile.close();
			}
		}
		pos = nextPos + boundary.length();
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
	return (S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode));
}

int request::matchLocation(server& _server)
{
	std::vector<Location> vec;
	vec = _server.getLocations();
	std::string paths = this->requestURI;
	
	std::vector<server> srvrs = _server.getSmSoServers();
	for (std::vector<server>::iterator it = srvrs.begin(); it != srvrs.end(); it++) {
		if (it->getServerName() == removewhites(this->headerFields["Host"])) {
			vec = it->getLocations();
		}
	}

	while (!paths.empty())
	{
		for (std::vector<Location>::iterator it = vec.begin(); it != vec.end(); it++) {
			if (it->getLocationName() == paths) {
				filePath = it->getRoot() + this->requestURI;
				if (!it->getRedirection().empty()) {
					redirectURL = it->getRedirection();
				}
				if (isDirectory(filePath.c_str())) {
					if (filePath[filePath.size() - 1] == '/')
						filePath = filePath + it->getIndex();
					if (!it->getRedirection().empty())
						redirectURL = it->getRedirection();
				}
				loc = *it;
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
	if (request.empty()) {
		return 1;
	}
	if (headerFields.find("Transfer-Encoding") != headerFields.end()) {
		cout << MAGENTA << "UPLOAD WITH CHuNKED <>" << RESET_TEXT << endl;
		parseChunked(request, _server);
	}
	else {
		if (headerFields.find("Content-Type") != headerFields.end()
		&& headerFields["Content-Type"].find("multipart/form-data") != std::string::npos) {
			cout << MAGENTA << "UPLOAD WITH BOuNDARY <>" << RESET_TEXT << endl;
			parseBoundary(request, _server);
			std::string extension = getFileExtension(this->requestURI);
			if (!extension.empty())
				this->ContentType = allContTypes[extension];
		}
		else {
			cout << MAGENTA << "UPLOAD WITH CONTENT LENGTH <>" << RESET_TEXT << endl;
			parseContentLength(request, _server);
		}
	}

	if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") == headerFields.end()
		&& chunkSize != 0) {
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") == std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& bodyContentLength < this->actualContentLength) {
			if (bodyContentLength < this->actualContentLength)
			return 0;
		}
	else if (headerFields["Transfer-Encoding"].find("chunked") != std::string::npos
		&& headerFields.find("Content-Length") != headerFields.end()
		&& chunkSize != 0) {
			return 0;
		}
	else {
		return 1;
	}
	return 1;
}


bool	request::getConnection()
{
	return this->keepAlive;
}

void	request::setConnection()
{
	if (headerFields.find("Connection") != headerFields.end())
	{
		if (headerFields["Connection"].find("keep-alive") != std::string::npos) {
			this->keepAlive = true;
		}
		else {
			this->keepAlive = false;
		}
	}
}

void request::setContentType()
{
	addAllContentTypes();

	if (this->method == "GET" || this->method == "POST") {
		if (isDirectory(requestURI.c_str())) {
			this->ContentType = "text/html";
		}
		else {
			std::string fileExtension;
			size_t dotPosition = requestURI.rfind(".");

			if (dotPosition != std::string::npos) {
				fileExtension = requestURI.substr(dotPosition);
			}
			this->ContentType = allContTypes[fileExtension];
			if (fileExtension == ".php" || fileExtension == ".py")
				this->ContentType = "";
		}
	}
	if (this->method == "POST") {
		if (this->headerFields.count("Content-Type") > 0 && !this->headerFields["Content-Type"].empty()){
			this->uploadContentType = this->headerFields["Content-Type"];
			this->boundary = "--" + this->uploadContentType.substr(this->uploadContentType.find("boundary=") + 9);
			size_t lastNonSpace = this->boundary.find_last_not_of(" \t\r\n");
			if (lastNonSpace != std::string::npos) {
				this->boundary.erase(lastNonSpace + 1);
			}
		}
	}
}

void	request::setContentType(std::string contentType)
{
	this->ContentType = contentType;
}

void	request::addAllContentTypes()
{
	allContTypes[".aac"] = "audio/aac";
	allContTypes[".abw"] = "application/x-abiword";
	allContTypes[".arc"] = "application/x-freearc";
	allContTypes[".avif"] = "image/avif";
	allContTypes[".avi"] = "video/x-msvideo";
	allContTypes[".azw"] = "application/vnd.amazon.ebook";
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

void request::getHeadersRequest(std::string requestPart) {
	if (requestPart.find("\r\n") != std::string::npos) {
		headersDone = headerFieldState;
	}
	if (requestPart.find("\r\n\r\n") != std::string::npos) {
		headersDone = headersDoneState;
	}
	if (this->headers.find("\r\n\r\n") != std::string::npos)
	{
		headersDone = headersDoneState;
	}
	if (requestPart.find("\r\n\r\n") == std::string::npos) {
		this->headers.append(requestPart);
		if (this->headers.find("\r\n\r\n") != std::string::npos)
		{
			headersDone = headersDoneState;
		}
		return ;
	}
	if (requestPart.find("\r\n\r\n") != std::string::npos) {
		this->headers.append(requestPart.substr(0, requestPart.find("\r\n\r\n")));
		if (this->headers.find("\r\n\r\n") != std::string::npos)
		{
			headersDone = headersDoneState;
		}
		return ;
	}
	return ;
}


int request::getBodyRequest(std::string requestPart)
{
	if (headersDone) {
		if (headersDone == headersDoneState) {
			bodyContentLength += requestPart.substr(requestPart.find("\r\n\r\n") + 4).size();
			this->theBody.append(requestPart.substr(requestPart.find("\r\n\r\n") + 4));
			headersDone = bodyState;
		}
		else
			this->theBody.append(requestPart);
		if (headerFields.find("Content-Type") != headerFields.end()
			&& headerFields["Content-Type"].find("multipart/form-data") != std::string::npos) {
				if (requestPart.find(boundary + "--") != std::string::npos) {
					return 1;
				}
		}
		if (headerFields.find("Transfer-Encoding") != headerFields.end()
			&& headerFields["Transfer-Encoding"].find("chunked") != std::string::npos) {
				if (requestPart.find("\r\n0\r\n\r\n") != std::string::npos) {
					return 1;
				}
		}
		else {
			if ((int)this->theBody.size() >= this->actualContentLength) {
				return 1;
			}
		}
	}
	return 0;
}

void	request::setCgiHeader(std::string s){
	this->cgi_header = s;
}
void	request::setCgiBody(std::string s){
	this->cgi_body = s;
}

string request::getCgiBody(){
	return this->cgi_body;
}
string request::getCgiHeader(){
	return this->cgi_header;
}