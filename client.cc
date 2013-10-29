#include <iostream>
#include <stdlib.h>

#include "socket.hh"
#include "client.hh"


Client::Client(std::string ipAddress, uint16_t portNumber)
: port(portNumber), addr(ipAddress, portNumber), sock(UDP)
{
    std::cout << "New client" << std::endl;
}

void Client::run()
{
	sock.sendto(addr, "Hello");
	while (true)
	{
  		std::pair<Address, std::string> rcvd = sock.recvfrom();
    	std::cout << "Sender: " << rcvd.first.str() << std::endl;
        std::cout << "Payload: " << rcvd.second << std::endl;
		usleep(1000);
	} 
}

int main(int argc, char *argv[]) {
   
   if (argc != 2)
	{
   		Client clt;
		/* std::cerr << "Usage: receiver <port>" << std::endl;
		    exit(1);*/
		clt.run();
   	}
	else
   	{
   		Client clt(argv[1],myatoi(argv[2]));
		clt.run();
   	}

}
