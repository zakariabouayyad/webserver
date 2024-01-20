#include "../server.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void removefd(vector<struct pollfd>& pfds, struct pollfd& pfd, server& server){
	int fd = pfd.fd;
	close(fd);
	server.clients.erase(fd);
	for(vector<pollfd>::iterator it = pfds.begin();it != pfds.end();it++){
		if (!memcmp(&(*it), &pfd, sizeof(struct pollfd))){
			cout<<RED<<"found IT: "<<it->fd<<RESET_TEXT<<endl;
			pfds.erase(it);
			break;
		}
	}
	vector<int>::iterator it = std::find(server.mysockets.begin(),
	server.mysockets.end(),	fd);
	if (it != server.mysockets.end())
		server.mysockets.erase(it);
}

void	fillpoll_listen(vector<struct pollfd>&	pfds, serversInfos _si){
	struct pollfd p;
	for (size_t i = 0;i < _si.allSockets.size();i++){
		p.fd = _si.allSockets[i];
		p.events = POLLIN | POLLHUP;
		pfds.push_back(p);
	}
}

void	accept_connection(vector<struct pollfd>& pfds, struct pollfd& pfd, server& server)
{
	struct sockaddr_storage client_addr;
	socklen_t clientaddr_len = sizeof(client_addr);
	int cs;

	cout<<"listen socket: "<<pfd.fd<<endl;
	cs = accept(pfd.fd, (sockaddr*)&client_addr, &clientaddr_len);
	fcntl(cs, F_SETFL,O_NONBLOCK, FD_CLOEXEC);
	cout << "connection socket: " << cs <<endl;
	server.mysockets.push_back(cs);
	struct pollfd p;
	p.fd = cs;
	p.events = POLLIN;
	client t;
	server.clients[p.fd] = t;
	pfds.push_back(p);
}

void readRequest(vector<struct pollfd>&	pfds,struct pollfd &pfd, server& server){
	char request[1024] = {0};
	string theRequest;

	if (pfd.revents & POLLHUP)
		removefd(pfds,pfd,server);
	else if (!server.clients[pfd.fd].getTookrequest())
	{
		// cout << "thisis socket connection: " << pfd.fd<<endl;
		int r = read(pfd.fd, request, 1024);
		if (r <= 0){
			perror("read");
			sleep(2);
		}
		// request[r] = '\0';
		theRequest = string(request, r);

		// cout << "Received " << r << " bytes." << endl;
		// printf("\033[1;37m%.*s\033[0m", r, request);
		server.clients[pfd.fd].set_request(theRequest, server);
		if (server.clients[pfd.fd].tookrequest == 1)
			pfd.events = POLLOUT | POLLHUP;
	}
}

void	sendResponse(vector<struct pollfd>&	pfds, struct pollfd &pfd, server& server)
{
	if (server.clients[pfd.fd].getTookrequest())
	{
		server.clients[pfd.fd].set_response(pfd.fd);
		if (server.clients[pfd.fd].getfilesent() || pfd.revents & POLLHUP)
			removefd(pfds, pfd, server);
	}
}

void	accept_read_write(vector<struct pollfd>&	pfds, struct pollfd &pfd,
	vector<server>& servers){
	for (size_t i = 0;i < servers.size();i++){
		if (std::find(servers[i].mysockets.begin(),
			servers[i].mysockets.end(), pfd.fd) != servers[i].mysockets.end())
		{
			if (pfd.revents & POLLIN){
				// printf("POLLIN: %d.\n", pfd.fd);
				if (pfd.fd == servers[i].get_slistener())
					accept_connection(pfds, pfd, servers[i]);
				else
					readRequest(pfds, pfd, servers[i]);
			}
			else if ((pfd.revents & POLLOUT)){
				// printf("POLLOUT : %d\n", pfd.fd);
				sendResponse(pfds, pfd, servers[i]);
			}
		}
	}
}

void	main_loop(vector<server> Confservers){
	serversInfos	_si(Confservers);
	_si.SetListener();
	vector<server> servers = _si.get_servers();

	//multiplexing v4.0 ~
	vector<struct pollfd>	pfds;
	fillpoll_listen(pfds, _si);
	struct pollfd*			p;

	while(1){
		p = pfds.data();
		if (poll(p, pfds.size(), -1) < 0){
			perror("poll");
			sleep(1);
		}
		for (size_t i = 0;i < pfds.size();i++)
			accept_read_write(pfds, pfds[i], servers);
	}
}