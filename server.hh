#ifndef SERVER_HH_
#define SERVER_HH_

class Server
{
public:
  Server( const uint16_t portNumber );
  void run( void );
  
private:
  uint16_t port;
  Address addr;
  Socket sock;
};

#endif /* SERVER_HH_ */
