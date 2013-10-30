#include <iostream>

#include "socket.hh"
#include "client.hh"

using namespace std;

Client::Client(string ipAddress, uint16_t portNumber)
: port(portNumber), addr(ipAddress, portNumber), sock(UDP)
{
    cout << "-- New Client --" << endl;
}

void Client::run()
{
    sock.sendto(addr, "Hello from Client");
	while (true)
	{
  		pair<Address, string> rcvd = sock.recvfrom();
    	cout << "Client received message '" << rcvd.second << "' from " << rcvd.first.str() << endl << endl;
		usleep(999999);
		sock.sendto(rcvd.first, "Hello from Client");
	} 
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cerr << "Usage: client <ip address> <port>" << endl;
		exit(1);
	}
	try
   	{ 
   		Client clt(argv[1],myatoi(argv[2]));
		clt.run();
	}
    catch (const Exception & e)
    {
        e.perror();
		return EXIT_FAILURE;
    }
}
