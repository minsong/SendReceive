#ifndef CLIENT_HH_
#define CLIENT_HH_

#include <string>

#include "whiskertree.hh"
#include "memory.hh"
#include "socket.hh"

class Client
{
public:
  Client( WhiskerTree & s_whiskers, const std::string dest_address, const std::string dest_service );
  int run( void );
  
private:
  const WhiskerTree & _whiskers;
  Memory _memory;
  bool _track;
  Address addr;
  Socket sock;

  void packet_received( const Packet &packet, const uint64_t &flow_id, uint64_t &largest_ack, uint64_t &the_window, uint64_t &intersend_time );
  uint64_t next_event_time( const uint64_t &last_datagram_sent, const uint64_t &intersend_time ) const;
};

#endif /* CLIENT_HH_ */
