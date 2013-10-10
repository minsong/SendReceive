#include <stdlib.h>
#include <iostream>
#include <cstring>     
#include <sys/socket.h>
#include <netdb.h>

const int BUFSIZE = 1024;

int main(int argc, char *argv[]) {

  	if (argc != 2) 
	{
    	std::cerr << "Usage: receiver <port>" << std::endl;
    	exit(1);
  	}

	int socketfd;
    struct sockaddr_in rcvaddr;

    memset(&rcvaddr, 0, sizeof(rcvaddr));
    rcvaddr.sin_family = AF_INET;
    rcvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    rcvaddr.sin_port = htons(atoi(argv[1]));

    if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		std::cerr << "Cannot create UDP socket" << std::endl;
		exit(2);
    }

  	if (bind(socketfd, (sockaddr *) &rcvaddr, sizeof(rcvaddr)) < 0) 
	{
		std::cerr << "Cannot bind to port" << std::endl;
		exit(3);
  	}

    struct sockaddr_in sndaddr;
	int bytesrcvd;
	socklen_t sndaddrlen = sizeof(sndaddr);
    char buffer[BUFSIZE];

	std::cout << "Waiting for data..." << std::endl;
    if ((bytesrcvd = recvfrom(socketfd, buffer, BUFSIZE, 0, (struct sockaddr*) &sndaddr, &sndaddrlen))< 0)
	{
		std::cerr << "Error receiving data" << std::endl;
		exit(4);
	}
    buffer[bytesrcvd] = '\0';

    std::cout << "Payload:" << buffer << std::endl;
  	return 0;
}

