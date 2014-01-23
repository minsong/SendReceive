#include <iostream>
#include <limits>

#include "client.hh"
#include "poller.hh"
#include "timestamp.hh" 

using namespace std;
using namespace PollerShortNames;

Client::Client( const string s_dest_address, const string s_dest_service, WhiskerTree & s_whiskers,  const bool s_track )
  : _addr( s_dest_address, s_dest_service, UDP ), 
    _sock( UDP ),
    _whiskers( s_whiskers ),
    _memory(),
    _track( s_track )
{
  cout << "-- New Client --" << endl;
}

int Client::run( void ){  

  uint64_t last_datagram_sent = 0, largest_ack = 0, the_window = 0, intersend_time = 0,  next_seqnum = 0, flow_id = 0;

  /* initial window and intersend time */
  const Whisker & current_whisker( _whiskers.use_whisker( _memory, _track ) );
  the_window = current_whisker.window( the_window );
  intersend_time = current_whisker.intersend();

  Poller poller;
  poller.add_action( Poller::Action( _sock.fd(),
				     Direction::In,
				     [&] () {
				       const auto received_packet = _sock.recv();
				       packet_received( received_packet, flow_id, largest_ack, the_window, intersend_time ); 

				       cout << "Received '" << received_packet.payload();
				       cout << "' with acknum " << received_packet.ack_number();
				       cout << " from " << received_packet.addr().str() << endl;
				       
				       return ResultType::Continue;
				     } ) );

  while ( true ) {
    /* Send packets every interval up to available window size */
    while ( next_seqnum < largest_ack + the_window ) {
      
      const uint64_t now = timestamp();
      if ( now >= next_event_time( last_datagram_sent, intersend_time ) ) {
	//TODO: flow_id set?
	Packet send_packet( _addr , 0, next_seqnum, 0, "Hello from Client" );
	_sock.send( send_packet );
        _memory.packet_sent( send_packet );

	cout << "Sent packet with seqnum " << send_packet.sequence_number();
	cout << " at time " << send_packet.send_timestamp();
	cout << " to " << send_packet.addr().str() << endl; 
	
	last_datagram_sent = now;
	next_seqnum++;
      }
    }
    //TODO: How is timer set?
    auto poll_result = poller.poll( 10000 );

    if ( poll_result.result == Poller::Result::Type::Timeout ) {
      cout << "Timed out." << endl;
      largest_ack = next_seqnum;
      //TODO: Do anything else on timeout?
    }
    else if ( poll_result.result == Poller::Result::Type::Exit ) {
      cout << "Quitting after a file descriptor received an error." << endl;
      return poll_result.exit_status;
    }
  }

  return EXIT_SUCCESS;
}

void Client::packet_received( const Packet &packet, const uint64_t &flow_id, uint64_t &largest_ack, uint64_t &the_window, uint64_t &intersend_time ) {
  /* Assumption: There is no reordering */
  largest_ack = max( packet.sequence_number(), largest_ack );
  _memory.packet_received( packet, flow_id );
  
  const Whisker & current_whisker( _whiskers.use_whisker( _memory, _track ) );
  
  the_window = current_whisker.window( the_window );
  intersend_time = current_whisker.intersend();
}

uint64_t Client::next_event_time( const uint64_t &last_datagram_sent, const uint64_t &intersend_time ) const 
{
  return last_datagram_sent + intersend_time;
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
    return clt.run();

  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
