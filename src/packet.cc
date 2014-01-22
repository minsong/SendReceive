#include <iostream>
#include <sys/time.h> 

#include "packet.hh"
#include "ezio.hh"
#include "timestamp.hh"

using namespace std;

Packet::Packet( const Address & addr, const uint64_t flow_id, const uint64_t sequence_number, const uint64_t ack_number, const string payload )
  : addr_( addr ),
    flow_id_( flow_id ),
    sequence_number_( sequence_number ),
    ack_number_( ack_number ),
    send_timestamp_(),
    echo_reply_timestamp_(),
    payload_( payload ),
    payload_len_( sizeof( payload ) )
{
}

Packet::Packet( const Address & addr, const string & str )
  : addr_( addr ),
    flow_id_(),
    sequence_number_(),
    ack_number_(),
    send_timestamp_(),
    echo_reply_timestamp_(),
    payload_(),
    payload_len_()
{
  flow_id_ = str.substr( 0*sizeof( uint64_t ), sizeof( uint64_t ) );
  sequence_number_ = str.substr( 1*sizeof( uint64_t ), sizeof( uint64_t ) );
  ack_number_ = str.substr( 2*sizeof( uint64_t ), sizeof( uint64_t ) );
  send_timestamp_ = str.substr( 3*sizeof( uint64_t ), sizeof( uint64_t ) );
  echo_reply_timestamp_ = str.substr( 4*sizeof( uint64_t ), sizeof( uint64_t ) );
  payload_len_ = str.substr( 5*sizeof(uint64_t), sizeof( uint64_t) );
  payload_ = str.substr( 6*sizeof( uint64_t ) );
}

string Packet::str( void ) const
{
  string ret = flow_id_.str()
    + sequence_number_.str()
    + ack_number_.str()
    + send_timestamp_.str()
    + echo_reply_timestamp_.str()
    + payload_len_.str()
    + payload_;
  
  return ret;
}

void Packet::set_send_timestamp( void ) 
{
  /* Fill in send timestamp */
  send_timestamp_ = Integer64( timestamp() ); 
}

void Packet::set_echo_reply_timestamp( uint64_t echo_timestamp  ) 
{
  /* Fill in echo reply timestamp */
  echo_reply_timestamp_ = Integer64( echo_timestamp ); 
}


