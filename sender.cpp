#include <stdlib.h>
#include <iostream>
#include <cstring>     
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

	std::cout << argv[0] << argv[1] << argv[2] << argv[3] << argc << std::endl;
	std::cout << strlen(argv[3]) << argc << std::endl;

	if (argc != 4) 
	{
		std::cerr << "Usage: " << "sender <address> <port> <message>" << std::endl;
		exit(1);
	}

    int socketfd, bytesent;
    struct sockaddr_in rcvaddr;

    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		std::cerr <<"Cannot create socket" << std::endl;
		exit(2);
    }

    memset(&rcvaddr, 0, sizeof(rcvaddr));
    rcvaddr.sin_family = AF_INET;
    rcvaddr.sin_addr.s_addr = inet_addr(argv[1]);
    rcvaddr.sin_port = htons(atoi(argv[2]));

    if ((bytesent = sendto(socketfd, argv[3], strlen(argv[3]), 0, (struct sockaddr*) &rcvaddr, sizeof(rcvaddr))) < 0) 
	{
		std::cerr <<"Cannot send data" << std::endl;
		exit(3);
    }

  	return 0;
}
