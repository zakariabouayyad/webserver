#include "../server.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
//

void	response::initialize(request& request){
	cout << RED<< request.getFilePath() << RESET_TEXT << endl;

	int fd = open(request.getFilePath().c_str(), O_RDONLY);
	filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	buffer = new char[filesize];
	int c = read(fd, buffer, filesize);
	cout <<"read file int: " << c << endl;
	close(fd);
}

void	response::sendHeader(int connection_socket, request& request){
	std::string header = "HTTP/1.1 200 OK\r\n"
		"Content-Length: " + std::to_string(filesize) + "\r\n"
		"Keep-Alive: timeout=1" + "\r\n"
		"Content-Type: "+ request.getContentType() + "\r\n\r\n"+'\0';

	write(connection_socket, header.c_str(),	//header
		strlen(header.c_str()));
}

int	response::sendBody(int connection_socket, request& request){
	(void) request;
	// cout << MAGENTA << request.getFilePath() << RESET_TEXT << endl;
	// cout << "connectionsocket: " << connection_socket << endl;
	size_t len = 1024;
	if (len > filesize - totalSent)
		len = filesize - totalSent;
	// cout << len << RESET_TEXT << endl;
	int bytes_sent = write(connection_socket, buffer + totalSent, len);
	if (bytes_sent <= 0){
		cout << "error\n";
		perror("write");
		totalSent++;
		sleep(2);
	}
	totalSent += bytes_sent;
	// cout << GREEN << totalSent << '/' << filesize << " sent" << RESET_TEXT  << endl;
	int allFileSent = 0;
	if (totalSent >= filesize){
		cout << WHITE<< "hey I've sent the whole file" << endl;
		allFileSent = 1;
		totalSent = 0;
		// free(buffer);
	}
	return allFileSent;
}

//SEND():
// FAILS with ->> 351608 Bytes left on socket buffer .
// WORKS FINE with ->> 638328 Bytes left on socket buffer .