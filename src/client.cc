#include <iostream>
#include <limits>

#include "client.hh"
#include "poller.hh"
#include "timestamp.hh" 
#include "utility.hh"

using namespace std;
using namespace PollerShortNames;

static const uint64_t K = 4.0;
static const double alpha = 1.0/8.0;
static const double beta = 1.0/4.0;

Client::Client( const string s_dest_address, 
		const string s_dest_service, 
		WhiskerTree & s_whiskers, 
		const bool s_track )
  : _addr( s_dest_address, s_dest_service, UDP ),
    _sock( UDP ),
    _whiskers( s_whiskers ),
    _memory(),
    _track( s_track )
{
  cout << "-- New Client --" << endl;
}

int Client::run( const uint64_t num_total_packets ){

  uint64_t last_packet_sent = 0, largest_ack = 0, the_window = 0, intersend_time = 0,  next_seqnum = 0, flow_id = 1;

  /* initial window and intersend time */
  set_window_intersend( the_window, intersend_time );

  /* Initial RTO = 1 second  */
  uint64_t rto = 1000;
  uint64_t srtt = 0, rttvar = 0;

  Utility utility;

  Poller poller;
  poller.add_action( Poller::Action( _sock.fd(),
				     Direction::In,
				     [&] () {
				       const auto received_packet = _sock.recv();
				       packet_received( received_packet, flow_id, largest_ack, the_window, intersend_time ); 
				       update_timeout( received_packet, srtt, rttvar, rto );
				       utility.packet_received( received_packet );

				       cout << "Received '" << received_packet.payload();
				       cout << "' with acknum " << received_packet.ack_number();
				       cout << " from " << received_packet.addr().str() << endl;
				       
				       return ResultType::Continue;
				     } ) );

  while ( next_seqnum < num_total_packets ) {

    const uint64_t start_sending = timestamp();

    /* Send packets every interval up to available window size */
    while ( next_seqnum < num_total_packets and next_seqnum < largest_ack + the_window ) {
      
      const uint64_t now = timestamp();
      if ( now >= next_event_time( last_packet_sent, intersend_time ) ) {
	//TODO: flow_id set?
	Packet send_packet( _addr , 0, next_seqnum, 0, "Hello from Client" );
	_sock.send( send_packet );
        _memory.packet_sent( send_packet );

	cout << "Sent packet with seqnum " << send_packet.sequence_number();
	cout << " at time " << send_packet.send_timestamp();
	cout << " to " << send_packet.addr().str() << endl;
	
	last_packet_sent = now;
	next_seqnum++;
      }
    }
    utility.sending_duration( timestamp() - start_sending );

    auto poll_result = poller.poll( rto );

    if ( poll_result.result == Poller::Result::Type::Timeout ) {
      cout << "Timed out." << endl;
      largest_ack = next_seqnum;
    }
    else if ( poll_result.result == Poller::Result::Type::Exit ) {
      cout << "Quitting after a file descriptor received an error." << endl;
      return poll_result.exit_status;
    }
  }

  cout << "Avg throughput: " << utility.average_throughput() << endl;
  cout << "Avg delay: " << utility.average_delay() << endl;
  cout << "Avg utility: " << utility.utility() << endl;
  return EXIT_SUCCESS;
}

void Client::set_window_intersend( uint64_t &the_window, uint64_t &intersend_time ) {
  const Whisker & current_whisker( _whiskers.use_whisker( _memory, _track ) );
  the_window = current_whisker.window( the_window );
  intersend_time = current_whisker.intersend();
}

void Client::packet_received( const Packet &packet, const uint64_t &flow_id, uint64_t &largest_ack, uint64_t &the_window, uint64_t &intersend_time ) {
  /* Assumption: There is no reordering */
  largest_ack = max( packet.sequence_number(), largest_ack );
  _memory.packet_received( packet, flow_id );
  
  set_window_intersend( the_window, intersend_time );
}

void Client::update_timeout( const Packet &received_packet, uint64_t &srtt, uint64_t &rttvar, uint64_t &rto ){
  const uint64_t received_time = timestamp();
  const uint64_t r = received_time - received_packet.echo_reply_timestamp();

  if ( !(srtt) ) {
    srtt = r;
    rttvar = r/2;
  }
  else {
    const uint64_t delta = srtt > r ? srtt-r : r-srtt;
    rttvar = ( 1 - beta )*rttvar + beta*delta;
    srtt = ( 1 - alpha )*srtt + alpha*r;
  }

  // TODO Clock granularity: rto = srtt + max( G, K*rttvar ); 
  rto = max( srtt + K*rttvar, ( uint64_t ) 1000 );
}

uint64_t Client::next_event_time( const uint64_t &last_packet_sent, const uint64_t &intersend_time ) const 
{
  return last_packet_sent + intersend_time;
}

int main( int argc, char *argv[] ) {
  try { 
    if ( argc <= 0 ) {
      throw Exception( "client", "Missing argv[ 0 ]" );
    }
    if ( argc != 4 ){
      throw Exception( argv[ 0 ] , "DEST_ADDRESS DEST_SERVICE FILENAME" );
    }

    WhiskerTree whiskers;
    string filename( argv[ 3 ] );
    int fd = open( filename.c_str(), O_RDONLY );
    if ( fd < 0 ) {
      throw Exception( argv[ 0 ], "Could not open file" );
    }
    RemyBuffers::WhiskerTree tree;
    if ( !tree.ParseFromFileDescriptor( fd ) ) {
      throw Exception( argv[ 0 ], "Could not parse file" );
    }
    whiskers = WhiskerTree( tree );
    
    Client clt( argv[ 1 ], argv[ 2 ], whiskers );
    return clt.run(1000);

  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
