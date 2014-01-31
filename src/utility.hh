#ifndef UTILITY_HH
#define UTILITY_HH

#include <cmath>
#include <limits>

class Utility
{
private:
  uint64_t _time_sending;
  uint64_t _packets_received;
  uint64_t _total_delay;

public:
  Utility( void ) : _time_sending( 0 ), _packets_received( 0 ), _total_delay( 0 ) {}

  void sending_duration( const uint64_t & duration ) { _time_sending += duration; }
  void packet_received( const Packet & packet ) {
    _packets_received++;
    
    const uint64_t received_time = timestamp();
    assert( received_time >= packet.echo_reply_timestamp() );
    _total_delay +=  received_time - packet.echo_reply_timestamp();
  }

  double average_throughput( void ) const
  {
    if ( _time_sending == 0 ) {
      return 0.0;
    }
    return double( _packets_received ) / _time_sending;
  }

  double average_delay( void ) const
  {
    if ( _packets_received == 0 ) {
      return 0.0;
    }
    return double( _total_delay ) / double( _packets_received );
  }

  double utility( void ) const
  {
    if ( _time_sending == 0 ) {
      return 0.0;
    }

    if ( _packets_received == 0 ) {
      return std::numeric_limits<double>::lowest();
    }

    const double throughput_utility = log2( average_throughput() );
    const double delay_penalty = log2( average_delay() / 100.0 );

    return throughput_utility - delay_penalty;
  }
};

#endif
