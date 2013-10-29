#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <cstring>

class Client
{
public:
	Client( std::string ipAddress = "localhost",  uint16_t portNumber = 1500 );
	void run(void);
private:
 	uint16_t port;
	Address addr;
	Socket sock;

};


#endif /* CLIENT_HH_ */
