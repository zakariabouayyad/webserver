#include "../server.hpp"
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

void	client::set_request(char* r, server& _server){
	std::ofstream output("theultimattest2", std::ios::app);
	output << r;
	output << "\n++++++++++++++++++++++++++++++++++++++++++\n";
    tookrequest = requestObj.parseRequest(r, _server);
	if (tookrequest)
		cout <<BLUE<<"took request is set to 1"<<RESET_TEXT <<endl;
	else
		cout <<BLUE<<"took request is set to 0"<<RESET_TEXT <<endl;

    // tookrequest = 1;
    // cout <<BLUE<<"took request is set to 1"<<RESET_TEXT <<endl;
}

void	client::set_response(int connection_socket){
    filesent = responseObj.set_res(connection_socket, requestObj);
    cout << RED <<"filesent: " << filesent << RESET_TEXT << endl;
    if (filesent == 1){
        tookrequest = 0;
        cout << BLUE<< "tookrequest is set to 0 again" << RESET_TEXT << endl;
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