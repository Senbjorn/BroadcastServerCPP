#ifndef BROADCASTSERVER_SERVER_H
#define BROADCASTSERVER_SERVER_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <list>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>


class Server {
typedef unsigned long id_type;
private:
	struct LocalClient;
	in_addr_t address;
	in_port_t port;
	struct sockaddr_in server_addr;
	std::atomic<id_type> next_id;
	
	std::mutex clients_mutex;
	std::unordered_map<id_type, std::shared_ptr<LocalClient>> clients;
public:
	Server() = delete;
	Server(const Server & other) = delete;
	Server & operator=(const Server & other) = delete;
	Server(in_addr_t addr, in_port_t p): address(addr), port(p){
		std::cout << addr << std::endl;
		next_id.store(0);
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = address;
		server_addr.sin_port = port;
	}

public:

	void run() {
		std::thread main_thread(&Server::do_work, this);
		main_thread.join();
	}	
	
	void do_work() {
		std::cout << "Server: do work" << std::endl;
		int server_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (server_socket < 0) {
			std::cout << "ServerError: can't open a socket." << std::endl;
			exit(1);
		}
		if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			std::cout << "ServerError: can't bind the socket with an address.";
			exit(1);
		}
		if (listen(server_socket, 10) < 0) {
			std::cout << "ServerError: can't listen." << std::endl;
		}
		std::cout << "Server: on accept." << std::endl;
		while (true) {
			struct sockaddr* client_addr;
			socklen_t* client_size;
			int client_socket = accept(server_socket, client_addr, client_size);
			if (client_socket < 0) {
				std::cout << "ServerError: accept failed." << std::endl;
				continue;
			}
			addClient(client_socket, client_addr);
			std::thread(&Server::client_worker, this, client_socket, client_addr, client_size);
		}
		std::cout << "Server: OK" << std::endl;
		close(server_socket);
	}

	void client_worker(int c_sock, struct sockaddr* c_addr, socklen_t* c_size) {
		std::cout << "ServerWorker: client accepted." << std::endl;
	}

	void addClient(int socket, struct sockaddr* address) {
		std::lock_guard<std::mutex> lock(clients_mutex);
		id_type id = next_id.load();
		next_id++;
		std::shared_ptr<LocalClient> sp(new LocalClient(socket, address, id));
		clients.insert(std::make_pair(id, sp));
	}

private:
	struct LocalClient {
		
		int socket;
		struct sockaddr* address;
		id_type id;

		LocalClient(int s, struct sockaddr* a, id_type i): socket(s), address(a), id(i) {}

	};
};


#endif
