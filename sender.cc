#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

#include "socket.hh"

int main(int argc, char *argv[]) {

    if (argc != 4)
    {
        std::cerr << "Usage: " << "sender <address> <port> <message>" << std::endl;
        exit(1);
    }
   
    try
    {
        Socket sock(UDP); 
	Address rcvaddr(argv[1],myatoi(argv[2]));
        sock.sendto(rcvaddr, argv[3]);
    }
    catch (const Exception & e)
    {
        e.perror();
	return EXIT_FAILURE;
    }

    return 0;
}
