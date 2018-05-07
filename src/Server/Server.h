#ifndef BROADCASTSERVER_SERVER_H
#define BROADCASTSERVER_SERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <list>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>

class Server {
typedef unsigned long id_type;
private:
	struct LocalClient;
	struct sockaddr_in server_addr;
	std::atomic<id_type> next_id;
	
	std::mutex clients_mutex;
	std::unordered_map<id_type, std::shared_ptr<LocalClient>> clients;
	std::unordered_map<id_type, std::shared_ptr<std::thread>> threads;
public:
	Server() = delete;
	Server(const Server & other) = delete;
	Server & operator=(const Server & other) = delete;
	Server(int p) {
		next_id.store(0);
		bzero((char *) &server_addr, sizeof(server_addr));
		struct hostent *server = gethostbyname("localhost");
                server_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr,
                         (char *)&server_addr.sin_addr.s_addr,
                         server->h_length);  
		server_addr.sin_port = htons(p);
	}
	Server(std::string host_name, in_port_t p) {
		next_id.store(0);
		char* name = new char[host_name.size() + 1];
                std::copy(host_name.begin(), host_name.end(), name);
                struct hostent *server = gethostbyname(name);
                server_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr,
                         (char *)&server_addr.sin_addr.s_addr,
                         server->h_length);
                server_addr.sin_port = htons(p);
	}

public:
	void run() {
		std::thread main_thread(&Server::do_work, this);
		main_thread.join();
	}
	
private:	
	void do_work() {
		std::cout << "Server: do work" << std::endl;
		int server_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (server_socket < 0) {
			std::cout << "ServerError: can't open a socket." << std::endl;
			exit(1);
		}
		std::cout << "Server: binding with IP=" 
			<< inet_ntoa(server_addr.sin_addr) 
			<< "; port="
			<< ntohs(server_addr.sin_port) << "." << std::endl;
		if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			std::cout << "ServerError: can't bind the socket with an address.";
			exit(1);
		}
		if (listen(server_socket, 10) < 0) {
			std::cout << "ServerError: can't listen." << std::endl;
			exit(1);
		}
		while (true) {
			std::cout << "Server: on accept." << std::endl;
			struct sockaddr client_addr;
			socklen_t client_size;
			int client_socket = accept(server_socket, &client_addr, &client_size);
			if (client_socket < 0) {
				std::cout << "ServerError: accept failed." << std::endl;
				continue;
			}
			std::cout << "ServerWorker: client accepted on socket " << client_socket << std::endl;
			auto client_ptr = addClient(client_socket, client_addr);
		}
		std::cout << "Server: OK" << std::endl;
		close(server_socket);
	}

	void client_worker(std::weak_ptr<LocalClient> c) {
		char msg[1024];
		size_t msg_len;
		while (auto client = c.lock()) {
			msg_len = recv(client->socket, (void *)msg, sizeof(msg), MSG_WAITALL);
			if (msg_len <= 0) {
				std::cout << "ServerWorkerError: recv failed." << std::endl;
				break;		
			}
			std::cout << "Message(client id="<< client->id << "): "<< msg << std::endl;
			broadcast(msg, sizeof(msg), client->id);
		}
		dropClient(c.lock());
	}
	void broadcast(const char *buffer, size_t buf_size, id_type from_id) {
			size_t buf_len;
			for (auto pair: clients) {
				auto id = pair.first;
				auto client = pair.second;
				if (id == from_id) continue;
				buf_len = send(client->socket, buffer, buf_size, MSG_DONTWAIT);
				if (buf_len < 0) {
					std::cout << "ServerError: writing to id=" << id << std::endl;
				}
			}
	}	
	void dropClient(std::shared_ptr<LocalClient> client) {
		if (client) {		
			std::lock_guard<std::mutex> lock(clients_mutex);
			close(client->socket);
			clients.erase(client->id);
		}
	}
	
	std::weak_ptr<LocalClient> addClient(int socket, struct sockaddr address) {
		std::lock_guard<std::mutex> lock(clients_mutex);
		id_type id = next_id.load();
		next_id++;
		std::shared_ptr<LocalClient> sp(new LocalClient(socket, address, id));
		clients.insert(std::make_pair(id, sp));
		auto client_ptr = std::weak_ptr<LocalClient>(sp);
		auto spt = std::shared_ptr<std::thread>(new std::thread(&Server::client_worker,
								this,
								client_ptr
		));
		threads.insert(std::make_pair(id, spt));
		return client_ptr;
	}

	struct LocalClient {
		
		int socket;
		struct sockaddr address;
		id_type id;

		LocalClient(int s, struct sockaddr a, id_type i): socket(s), address(a), id(i) {}

	};
};


#endif
