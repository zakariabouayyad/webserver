#include "../server.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void selectAndCopyset(fd_set& sockets, fd_set& copy, vector<int> allSockets){
	//struct pollfd pfds;
	FD_ZERO(&sockets);
	for (size_t i = 0; i < allSockets.size();i++){
		FD_SET(allSockets[i], &sockets);
		/*pfds[i].fd = allSockets[i];
    	pfds[i].events = POLLIN | POLLHUP;
		read -> pfds[i].events = POLLOUT | POLHUP*/
	}
	copy = sockets;
	int maxfd = *(std::max_element(allSockets.begin(), allSockets.end()));
	if (select(maxfd + 1, &copy, 0, 0, 0) < 0){
		perror("select failed");
		exit(EXIT_FAILURE);
	}
}

int	receiving(int connectionSocket, server server, map<int,client>& clients,vector<int>& toRemove){
	char request[1024];
	int bytesrecv = read(connectionSocket, request, 1024);
	if (!bytesrecv || bytesrecv < 0){
		cout << "bytesrecv: " << bytesrecv << endl;
		return 0;
	}
	clients[connectionSocket].setclient(request, connectionSocket,
		server);
	if (!clients[connectionSocket].getfilesent()){
		cout << "to be removed: " << connectionSocket << endl;
		toRemove.push_back(connectionSocket);
	}
	else
		return -1;
	return 1;
}

void	main_loop(vector<server> Confservers){
	serversInfos	_si(Confservers);
	_si.SetListener();
	vector<server> servers = _si.get_servers();
	vector<int> toRemove;
	map<int, client> clients;
	struct sockaddr_storage client_addr;
	socklen_t clientaddr_len = sizeof(client_addr);

	//multiplexing v3.2
	while (true){
		fd_set copy, sockets;
		selectAndCopyset(sockets, copy, _si.allSockets);
		for (size_t i = 0; i < servers.size();i++){
			for (size_t j = 0; j < servers[i].serversockets.size();j++){
			if (FD_ISSET(servers[i].serversockets[j], &copy)){
				int fd = servers[i].serversockets[j];
				if (fd == servers[i].get_slistener()){
					cout << "listener: " << fd << endl;
					servers[i].set_sconnection(accept(servers[i].get_slistener(),
							(sockaddr*)&client_addr, &clientaddr_len));
					fcntl(servers[i].get_sconncetion(),F_SETFL,O_NONBLOCK,FD_CLOEXEC);
					cout << "connection socket: " << servers[i].get_sconncetion()<<endl;;
					_si.allSockets.push_back(servers[i].get_sconncetion());
					servers[i].connectionsockets.push_back(servers[i].get_sconncetion());
					servers[i].serversockets.push_back(servers[i].get_sconncetion());
					client temp;
					clients[servers[i].get_sconncetion()] = temp;
				}
				// cout << "whatspopping!!!\n";
				else if (std::find(servers[i].connectionsockets.begin(),
					servers[i].connectionsockets.end(), fd) != servers[i].connectionsockets.end()
						&& !clients[fd].getfilesent()){
				int n = receiving(fd, servers[i], clients, toRemove);
				if (!n){
					cout << "read error " << endl;
					close(fd);
					FD_CLR(fd ,&copy);
					FD_CLR(fd ,&sockets);
					if (std::find(_si.allSockets.begin(),
					_si.allSockets.end(), fd) != _si.allSockets.end())
					_si.allSockets.erase(std::find(_si.allSockets.begin(),
						_si.allSockets.end(), fd));
					break ;
				}
				else if (n == -1){
					cout<<"remove this fd because file sentall=1: "<<fd<<endl;
					close(fd);
					FD_CLR(fd ,&copy);
					FD_CLR(fd ,&sockets);
					_si.allSockets.erase(std::find(_si.allSockets.begin(),
						_si.allSockets.end(), fd));
				}
				cout << "out of response" << endl;
				}
			}
		}
		}
			for (vector<int>::iterator i = toRemove.begin();i != toRemove.end();i++){
				cout << toRemove.size() << " fd clear: " << *i << endl;
				close(*i);
				FD_CLR(*i ,&copy);
				FD_CLR(*i ,&sockets);
				if (std::find(_si.allSockets.begin(),
					_si.allSockets.end(), *i) != _si.allSockets.end())
				_si.allSockets.erase(std::find(_si.allSockets.begin(),
					_si.allSockets.end(), *i));
			}
			toRemove.clear();
	}
}