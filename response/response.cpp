#include "../server.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
//

int	response::set_res(int connection_socket, request& request){
	cout << "in RESPONSE " << endl;
	static size_t O;
	size_t offset = O;
	cout << RED << "this is fileOFFSET: " << offset << RESET_TEXT << endl;
	fcntl(connection_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int bytes_sent;
	string filePath = request.getFilePath();
	string contentType = request.getContentType();
	std::ifstream File(filePath);
	int fd = open(filePath.c_str(), O_RDONLY);
	size_t filesize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char* buffer = (char*)malloc(filesize + 1);
	size_t c = read(fd, buffer, filesize);
	std::string header = "HTTP/1.1 200 OK\r\n"
		"Content-Length: " + std::to_string(filesize) + "\r\n"
		"Keep-Alive: timeout=1" + "\r\n"
		"Content-Type: "+ contentType + "\r\n\r\n";
	bytes_sent = send(connection_socket,
		header.c_str(), strlen(header.c_str()), 0);//header
	size_t total;
	total = offset;
	bytes_sent = 0;
	size_t len = 1024;
	cout <<"read file int: " << c << endl;
	while (total <= filesize){
		if (len > filesize - total)
			len = filesize - total;
		bytes_sent = write(connection_socket, buffer + total, len);
		// cout << "len: "<<len << endl;
		// usleep(100);
		if ((bytes_sent <= 0)){
			cout << "failed to write" << endl;
			total += bytes_sent;
			offset = total;
			break ;
		}
		total += bytes_sent;
		offset = total;
	}
	free(buffer);
	close(fd);
	cout << YELLOW << "RESPONSE SENT" << RESET_TEXT << endl;
	cout << GREEN << "this is file: " << filePath << RESET_TEXT << endl; 
	cout << GREEN << offset << '/' << filesize << " sent" << RESET_TEXT  << endl;
	O = offset;
	int allFileSent = 0;
	if (offset >= filesize){
		cout << WHITE<< "hey I've sent the whole file" << endl;
		allFileSent = 1;
		O = 0;
	}
	return allFileSent;
}

string    response::get_res(){
    return res;
}
//SEND():
// FAILS with ->> 351608 Bytes left on socket buffer .
// WORKS FINE with ->> 638328 Bytes left on socket buffer .