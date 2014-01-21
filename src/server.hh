#ifndef SERVER_HH_
#define SERVER_HH_

#include <string>

class Server
{
public:
  Server( const std::string local_service );
  int run( void );
  
private:
  Address addr;
  Socket sock;
};

#endif /* SERVER_HH_ */
