#ifndef PACKET_HH
#define PACKET_HH

#include <string>

#include "address.hh"

class Packet {
public:
  Packet( const Address & addr, const uint64_t sequence_number, const uint64_t ack_number, const std::string payload );

  Packet( const Address & addr, const std::string & str );
	
  std::string str( void ) const;
  void set_timestamp( void );

  const Address & addr( void ) const { return addr_; }
  uint64_t sequence_number( void ) const { return sequence_number_; }
  uint64_t ack_number( void ) const { return ack_number_; }
  uint64_t send_timestamp( void ) const { return send_timestamp_; }
  std::string payload( void ) const { return payload_; }
  uint64_t payload_len( void ) const { return payload_len_; }

private:
  Address addr_;
  uint64_t sequence_number_;
  uint64_t ack_number_;
  uint64_t send_timestamp_;
  std::string payload_;
  uint64_t payload_len_;
};

#endif
