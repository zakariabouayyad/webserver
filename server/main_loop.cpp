#include "../server.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void	fillpoll_listen(vector<struct pollfd>&	pfds, serversInfos _si){
	struct pollfd p;
	for (size_t i = 0;i < _si.allSockets.size();i++){
		p.fd = _si.allSockets[i];
		p.events = POLLIN;
		pfds.push_back(p);
	}
}

void	accept_connection(vector<struct pollfd>& pfds, int fd, server& server,
	serversInfos& _si)
{
	struct sockaddr_storage client_addr;
	socklen_t clientaddr_len = sizeof(client_addr);
	int cs;

	cout<<"listen socket: "<<fd<<endl;
	cs = accept(fd, (sockaddr*)&client_addr, &clientaddr_len);
	fcntl(cs,F_SETFL,O_NONBLOCK,FD_CLOEXEC);
	cout << "connection socket: " << cs <<endl;
	_si.allSockets.push_back(cs);
	server.connectionsockets.push_back(cs);
	// server.serversockets.push_back(cs);
	struct pollfd p;
	p.fd = cs;
	p.events = POLLIN;
	pfds.push_back(p);
	server.tempsm3 = server.get_slistener();
	server.set_slistener(-1);
}

void readRequest(struct pollfd &pfd, server& server, map<int, client>& clients){
	char request[1024];

	if (std::find(server.connectionsockets.begin(),
		server.connectionsockets.end(), pfd.fd) != server.connectionsockets.end()
		&& !clients[pfd.fd].getTookrequest())
	{
		int r = read(pfd.fd, request, 1024);
		std::ofstream outfile("outfile", std::ios::app);
		outfile << request;
		outfile << "\n++++++++++++++++++++++++ ";
		outfile << r;
		outfile << "\n";
		outfile.close();
		// cout << "Received " << r << " bytes." << endl;
		// printf("\033[1;37m%.*s\033[0m", r, request);
		// if (r > 0){
			// request[r] = '\0';
			clients[pfd.fd].set_request(request, server);
		// sleep(5);
			// cout<<"mawslatsh"<<endl;
			if (clients[pfd.fd].getTookrequest())
				pfd.events = POLLOUT;
		// }
	}

}

void	sendResponse(vector<struct pollfd>&	pfds, struct pollfd &pfd, server& server,
		map<int,client>& clients)
{
	(void)pfds;
	if ((pfd.revents & POLLOUT) && pfd.fd != server.get_slistener()
		&& clients[pfd.fd].getTookrequest())
	{
		if (std::find(server.connectionsockets.begin(),
			server.connectionsockets.end(), pfd.fd) != server.connectionsockets.end())
		{
			clients[pfd.fd].set_response(pfd.fd);
			if (clients[pfd.fd].getfilesent())
			{
				close(pfd.fd);
				server.set_slistener(server.tempsm3);
			}
		}
	}
	// else if (pfd.revents & POLLHUP){
	// 	cout << "POLLHUP " << pfd.fd << endl;
		// close(pfd.fd);
		// exit(0);
	// }
	// else if (pfd.revents & POLLNVAL)
	// 	cout << "nothing--> " << pfd.fd << endl;
}

void	accept_read_write(vector<struct pollfd>&	pfds, struct pollfd &pfd,
	vector<server>& servers, serversInfos& _si, map<int, client>& clients){
	for (size_t i = 0;i < servers.size();i++){
		if (pfd.revents & POLLIN){
			if (pfd.fd == servers[i].get_slistener())
				accept_connection(pfds, pfd.fd, servers[i], _si);
			else
				readRequest(pfd, servers[i], clients);
		}
		else if ((pfd.revents & POLLOUT) || (pfd.revents & POLLHUP))
			sendResponse(pfds, pfd, servers[i], clients);
	}
}

void	main_loop(vector<server> Confservers){
	serversInfos	_si(Confservers);
	_si.SetListener();
	vector<server> servers = _si.get_servers();
	map<int, client>	clients;
	//multiplexing v4.0 ~
	vector<struct pollfd>	pfds;
	fillpoll_listen(pfds, _si);
	struct pollfd*			p;

	while(1){
		p = pfds.data();
		poll(p, pfds.size(), -1);
		for (size_t i = 0;i < pfds.size();i++)
			accept_read_write(pfds, pfds[i], servers, _si, clients);
	}
}