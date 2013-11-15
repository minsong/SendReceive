#include <iostream>
#include <sys/time.h> 

#include "packet.hh"
#include "ezio.hh"

using namespace std;

Packet::Packet( const Address & addr, const uint64_t sequence_number, const uint64_t ack_number, const string payload )
  : addr_( addr ),
    sequence_number_( sequence_number ),
    ack_number_( ack_number ),
    send_timestamp_(),
    payload_( payload ),
    payload_len_( sizeof( payload ) )
{
}

Packet::Packet( const Address & addr, const string & str )
  : addr_( addr ),
    sequence_number_(),
    ack_number_(),
    send_timestamp_(),
    payload_(),
    payload_len_()
{
  sequence_number_ = myatoi(str.substr( 0*sizeof( uint64_t ), sizeof( uint64_t ) ).data());
  ack_number_ = myatoi(str.substr( 1*sizeof( uint64_t ), sizeof( uint64_t ) ).data());
  send_timestamp_ = myatoi(str.substr( 2*sizeof( uint64_t ), sizeof( uint64_t ) ).data());
  payload_len_ = myatoi(str.substr( 3*sizeof(uint64_t), sizeof(uint64_t) ).data());
  payload_ = str.substr( 4*sizeof(uint64_t) );
}

string Packet::str( void ) const
{
  string seq_num = to_string(sequence_number_);
  seq_num.resize( sizeof(uint64_t) );
  string ack_num = to_string(ack_number_);
  ack_num.resize( sizeof(uint64_t) );
  string timestamp = to_string(send_timestamp_);
  timestamp.resize( sizeof(uint64_t) );
  string payload_len = to_string(payload_len_);
  payload_len.resize( sizeof(uint64_t) );
  
  string ret = seq_num
    + ack_num
    + timestamp
    + payload_len
    + payload_;
  
  return ret;
}

void Packet::set_timestamp( void ) 
{
  timeval time;
  gettimeofday(&time, NULL);
  send_timestamp_ = (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

