#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <cstring>

class Client
{
public:
  Client( const std::string ipAddress, const uint16_t portNumber );
  void run( void );
  
private:
  uint16_t port;
  Address addr;
  Socket sock;
};

#endif /* CLIENT_HH_ */
