#include "../server.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void removefd(vector<struct pollfd>& pfds, struct pollfd& pfd, server& server){
	int fd = pfd.fd;
	server.clients[fd].reset();
	close(fd);
	server.clients.erase(fd);
	for(vector<pollfd>::iterator it = pfds.begin();it != pfds.end();it++){
		if (!memcmp(&(*it), &pfd, sizeof(struct pollfd))){
			cout << "removing this fd!: " << it->fd <<endl;
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
		p.events = POLLIN;
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
	char request[1025];
	string theRequest;

	if (!server.clients[pfd.fd].getTookrequest())
	{
		int r = read(pfd.fd, request, 1024);
		if (r <= 0){
			perror("read");
			cout << "removing fd now.."<<endl;
			removefd(pfds,pfd,server);
			return;
		}
		request[r] = '\0';
		theRequest = string(request, r);
		cout << WHITE << theRequest << RESET_TEXT << endl;
		server.clients[pfd.fd].set_request(theRequest, server);
		if (server.clients[pfd.fd].tookrequest == 1)
			pfd.events = POLLOUT;
	}
}

void	sendResponse(vector<struct pollfd>&	pfds, struct pollfd &pfd, server& server)
{
	if (server.clients[pfd.fd].getTookrequest())
	{
		if (!server.clients[pfd.fd].set_response(pfd.fd) ||
			(server.clients[pfd.fd].getfilesent() && !server.clients[pfd.fd].keepAlive))
			removefd(pfds, pfd, server);
		else if (server.clients[pfd.fd].getfilesent() && server.clients[pfd.fd].keepAlive){
			server.clients[pfd.fd].reset();
			pfd.events = POLLIN;
		}
	}
}

void	accept_read_write(vector<struct pollfd>&	pfds, struct pollfd &pfd,
	vector<server>& servers)
{
	for (size_t i = 0;i < servers.size();i++){
		if (std::find(servers[i].mysockets.begin(),
			servers[i].mysockets.end(), pfd.fd) != servers[i].mysockets.end())
		{
			if (pfd.revents & POLLIN)
			{
				if (pfd.fd == servers[i].get_slistener())
					accept_connection(pfds, pfd, servers[i]);
				else{
					try
					{
						readRequest(pfds, pfd, servers[i]);
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						internalServerError(pfd.fd);
						return ;
					}
				}
			}
			else if (pfd.revents & POLLOUT)
			{
				try
				{
					sendResponse(pfds, pfd, servers[i]);
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << endl;
					internalServerError(pfd.fd);
				}
				
			}
			else if (pfd.revents & POLLHUP)
				removefd(pfds, pfd, servers[i]);
		}
	}
}

void	checkTimeout(vector<struct pollfd>&	pfds, vector<server>& servers)
{
	for (size_t j = 0;j < pfds.size();j++){
		for (size_t i = 0;i < servers.size();i++){
			if (std::find(servers[i].mysockets.begin(),
			servers[i].mysockets.end(), pfds[j].fd) != servers[i].mysockets.end()
			&& pfds[j].fd != servers[i].get_slistener())
			{
				time_t t = servers[i].clients[pfds[j].fd].resTime;
				time_t diff = time(0) - t;
				if (diff >= 60){
					cout<<YELLOW<<"time's Up,removing client.. "<<pfds[j].fd<<RESET_TEXT<< endl;
					removefd(pfds, pfds[j], servers[i]);
				}
				else
					cout << "Not Yet!\n";
			}
		}
	}
}

void	main_loop(vector<server> Confservers){
	serversInfos	_si(Confservers);
	_si.SetListener();
	vector<server> servers = _si.get_servers();

	//multiplexing v10.01 ~
	vector<struct pollfd>	pfds;
	fillpoll_listen(pfds, _si);
	struct pollfd*			p;

	while(1){
		p = pfds.data();
		int r = poll(p, pfds.size(), 60000);
		if (r < 0){
			perror("poll");
			exit(EXIT_FAILURE);
		}
		else if (r == 0)
			checkTimeout(pfds, servers);
		for (size_t i = 0;i < pfds.size();i++)
				accept_read_write(pfds, pfds[i], servers);
	}
}