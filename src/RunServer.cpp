#ifndef BROADCASTSERVER_RUNSERVER
#define BROADCASTSERVER_RUNSERVER

#include <iostream>
#include "Server.h"
#include "netdb.h"

int main() {
	//char* addr = gethostbyname("localhost")->h_addr;
	//memcpy(&ipa.sin_addr.s_addr, addr, sizeof addr);
	Server server(*gethostbyname("localhost")->h_addr, 8000);
	server.run();
	std::cout << "RunServer: OK" << std::endl;
}

#endif
