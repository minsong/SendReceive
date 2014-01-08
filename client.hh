#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <string>

#include "socket.hh"

class Client
{
public:
  Client( const std::string dest_address, const std::string dest_service );
  int run( void );
  
private:
  Address addr;
  Socket sock;
};

#endif /* CLIENT_HH_ */
