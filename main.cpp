#include "server.hpp"
#include "aubb/Config.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main(int ac, char **av){
	if (ac != 2)
		return 0;
	try{
		Config	conf(av[1]);
		// signal(SIGPIPE, SIG_IGN);
		main_loop(conf.Servers);
	}
	catch(const std::exception& e){
		std::cerr << e.what() << endl;
	}
	
}
