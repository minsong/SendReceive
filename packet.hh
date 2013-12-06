#ifndef PACKET_HH
#define PACKET_HH

#include <string>

#include "address.hh"
#include "int64.hh"

/* Packet class */
class Packet {
public:
  Packet( const Address & addr, const uint64_t sequence_number, const uint64_t ack_number, const std::string payload );
  
  Packet( const Address & addr, const std::string & str );
  
  std::string str( void ) const;
  void set_timestamp( void );
    
  const Address & addr( void ) const { return addr_; }
  uint64_t sequence_number( void ) const { return sequence_number_.int64(); }
  uint64_t ack_number( void ) const { return ack_number_.int64(); }
  uint64_t send_timestamp( void ) const { return send_timestamp_.int64(); }
  std::string payload( void ) const { return payload_; }
  uint64_t payload_len( void ) const { return payload_len_.int64(); }
  
private:
  Address addr_;
  Integer64 sequence_number_;
  Integer64 ack_number_;
  Integer64 send_timestamp_;
  std::string payload_;
  Integer64 payload_len_;
};

#endif
