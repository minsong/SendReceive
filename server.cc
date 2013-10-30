#include <iostream>
#include <stdlib.h>
#include <cstring>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"
#include "server.hh"

const int BUFSIZE = 1024;

using namespace std;

Server::Server( uint16_t portNumber )
: port(portNumber), addr("127.0.0.1", portNumber), sock(UDP)
{
	try
   	{
		sock.bind(addr);
	}
    catch (const Exception & e)
    {
        e.perror();
		exit(1);
    }
	cout << "Server port number: " << port << endl;
}

void Server::run()
{
   while (true)
   {
      pair<Address, string> rcvd = sock.recvfrom();
    	cout << "Server received message '" << rcvd.second << "' from " << rcvd.first.str() << endl << endl;
	  sock.sendto(rcvd.first, "Hello from Server");
   } 
}

int main(int argc, char *argv[]) 
{
	if (argc != 2)
	{
		cerr << "Usage: server <port>" << endl;
		exit(2);
	}
	try
   	{ 
		Server svr(myatoi(argv[1]));
		svr.run();
	}
    catch (const Exception & e)
    {
        e.perror();
		return EXIT_FAILURE;
    }
}
