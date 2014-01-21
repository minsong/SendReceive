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
  
  
  void update_timeout( const Packet &received_packet, uint64_t &srtt, uint64_t &rttvar, uint64_t &rto );
  void update_window_size( const uint64_t sstresh, uint64_t &ca_incr, uint64_t &cwnd );
};

#endif /* CLIENT_HH_ */
