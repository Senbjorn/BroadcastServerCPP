#ifndef BROADCASTSERVER_RUNCLIENT
#define BROADCASTSERVER_RUNCLINET

#include <iostream>
#include <netdb.h>
#include "Client.h"

int main() {
	const char * name = "localhost";
	Client client(name, 8000);
	client.run();
	std::cout << "RunClient: OK" << std::endl;
}

#endif
