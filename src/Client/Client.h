#ifndef BROADCASTSERVER_CLIENT_H
#define BROADCASTSERVER_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>


class Client {
private:
	struct sockaddr_in server_addr;
	
public:
	Client() = delete;
	Client(const Client& other) = delete;
	Client& operator=(const Client* other) = delete;

	Client(std::string host_name, int p) {
		char* name = new char[host_name.size() + 1];
		std::copy(host_name.begin(), host_name.end(), name);
		struct hostent *server = gethostbyname(name);
		bzero((char *) &server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			 (char *)&server_addr.sin_addr.s_addr,
			 server->h_length);	
		server_addr.sin_port = htons(p);
	}
	
	void run() {
		std::thread main_thread(&Client::do_work, this);
		main_thread.join();
	}

private:	
	void do_work() {
		std::cout << "Server: do work" << std::endl;
                int client_socket = socket(AF_INET, SOCK_STREAM, 0);
                if (client_socket < 0) {
                        std::cout << "ClientError: can't open a socket." << std::endl;
                        exit(1);
                }
		std::cout << "Server: connect with IP=" 
                        << inet_ntoa(server_addr.sin_addr)
                        << "; port="
                        << ntohs(server_addr.sin_port) << "." << std::endl;
		if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			std::cout << "ClientError: can't connect to server." << std::endl;
			exit(1);
		}
		std::thread writer(&Client::write_to_server, this, client_socket);
		char msg[1024];
		size_t msg_len;
		while (true) {
			msg_len = read(client_socket, (void *)msg, sizeof(msg));
			if (msg_len <= 0) {
				std::cout << "ClientError: recv failed." << std::endl;
				break;
			}
			std::cout << "From server: " << msg << std::endl;
		}
	}

	void write_to_server(int client_socket) {
		char buffer[1024];
		size_t buf_len;
		while (true) {
			std::cout << ">>";
			bzero(buffer, 1024);
			fgets(buffer, 1023, stdin);
			buf_len = send(client_socket, buffer, sizeof(buffer), MSG_DONTWAIT);
			if (buf_len < 0) { 
				std::cout << "ClientError: writing to socket." << std::endl;
			}
		}
	}

};

#endif
