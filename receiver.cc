#include <stdlib.h>
#include <iostream>
#include <cstring>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"

const int BUFSIZE = 1024;

int main(int argc, char *argv[]) {

      if (argc != 2)
    {
        std::cerr << "Usage: receiver <port>" << std::endl;
        exit(1);
      }

    struct sockaddr_in rcvaddr;

    memset(&rcvaddr, 0, sizeof(rcvaddr));
    rcvaddr.sin_family = AF_INET;
    rcvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    rcvaddr.sin_port = htons(atoi(argv[1]));
    
    try
    {
        Socket sock(UDP);
        Address addr(rcvaddr);
        sock.bind(rcvaddr);

        std::cout << "Waiting for data..." << std::endl;
        std::pair<Address, std::string> rcvd = sock.recvfrom();
        std::cout << "Sender: " << rcvd.first.str() << std::endl;
        std::cout << "Payload: " << rcvd.second << std::endl;
    }
    catch (const Exception & e)
    {
        e.perror();
	_exit( EXIT_FAILURE);
    }

      return 0;
}
