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
   
    struct sockaddr_in rcvaddr;
    memset(&rcvaddr, 0, sizeof(rcvaddr));
    rcvaddr.sin_family = AF_INET;
    rcvaddr.sin_addr.s_addr = inet_addr(argv[1]);
    rcvaddr.sin_port = htons(atoi(argv[2]));
   
    try
    {
        Socket sock(UDP);
        Address addr(rcvaddr);
        sock.sendto(rcvaddr, argv[3]);
    }
    catch (const Exception & e)
    {
        e.perror();
	_exit( EXIT_FAILURE);
    }

    return 0;
}
