#ifndef SERVER_HH_
#define SERVER_HH_

class Server
{
public:
	Server( uint16_t portNumber = 1500 );
	void run();

private:
	uint16_t port;
	Address addr;
	Socket sock;
};


#endif /* SERVER_HH_ */
