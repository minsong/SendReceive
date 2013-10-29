#include <iostream>
#include <stdlib.h>
#include <cstring>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"
#include "server.hh"

const int BUFSIZE = 1024;

Server::Server( uint16_t portNumber )
: port(portNumber), addr("127.0.0.1", portNumber), sock(UDP)
{
   std::cout << "Port number: " << port << std::endl;
   sock.bind(addr);
}

void Server::run()
{
	while (true)
	{
  		std::pair<Address, std::string> rcvd = sock.recvfrom();
    	std::cout << "Sender: " << rcvd.first.str() << std::endl;
        std::cout << "Payload: " << rcvd.second << std::endl;
	} 
}

int main(int argc, char *argv[]) {

	try
   	{ 	
		if (argc != 2)
		{
			//for testing
			Server svr;
			svr.run();
		
			/* std::cerr << "Usage: receiver <port>" << std::endl;
			   exit(1);*/

	   	}
	  	else
	   	{
			Server svr(myatoi(argv[1]));
			svr.run();
	   	}	
   	}

    catch (const Exception & e)
    {
		std::cout << "--error--" << std::endl;
        e.perror();
		return EXIT_FAILURE;
    }
}
