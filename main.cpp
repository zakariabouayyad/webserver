#include "server.hpp"
#include "Parsing/Config.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int ac, char **av){
	try{
		Config	conf(ac, av);
		signal(SIGPIPE, SIG_IGN);
		main_loop(conf.Servers);
	}
	catch(const std::exception& e){
		std::cerr << e.what() << endl;
	}
}