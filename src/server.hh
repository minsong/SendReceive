#ifndef SERVER_HH_
#define SERVER_HH_

#include <string>

class Server
{
public:
  Server( const std::string s_local_service );
  int run( void );
  
private:
  Address _addr;
  Socket _sock;
};

#endif /* SERVER_HH_ */
