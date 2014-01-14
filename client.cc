#include <iostream>
#include <limits>

#include "client.hh"
#include "poller.hh"
#include "timestamp.hh"

using namespace std;
using namespace PollerShortNames;

Client::Client( const string dest_address, const string dest_service  )
  : addr( dest_address, dest_service, UDP ), sock( UDP )
{
  cout << "-- New Client --" << endl;
}

int Client::run( void ){  

  /* Currently using fixed 1s intervals */
  const int interval_ms = 1000;

  /* Keep track of last time we sent an outgoing datagram */
  uint64_t last_datagram_sent_ms = timestamp();

  uint16_t cwnd = 1;
  uint16_t ca_incr = 0;
  uint16_t ssthresh = numeric_limits<int>::max();
  uint16_t base = 0;
  uint16_t next_seqnum = 0; 

 /* Set up the events that we care about */
  Poller poller;

  /*Receive a packet */
  poller.add_action( Poller::Action( sock.fd(),
				     Direction::In,
				     [&] () {
				       const auto received_packet = sock.recv();
				       cout << "Received '" << received_packet.payload();
				       cout << "' with acknum " << received_packet.ack_number();
				       cout << " at time " << timestamp();
				       cout << " from " << received_packet.addr().str() << endl;
				       
				       /* Slow start */ 
				       if (cwnd < ssthresh){
					 cwnd++;
					 //cout << "Slow start-> incremented cwnd: " << cwnd << endl;
				       }
				       /* Congestion avoidance: cwnd >= ssthresh */
				       else {
					 /* Count up to cwnd, then increment cwnd */
					 if ( ++ca_incr % cwnd  == 0 ){
					   cwnd++;
					   ca_incr = 0;
					   //cout << "Congestion avoidance-> incremented cwnd: " << cwnd << endl;
					 }
				       }

				       base = received_packet.ack_number();
				       
				       return ResultType::Continue;
				     } ) );

  while ( true ) {
    /* Currently using fixed 10s timeout */
    uint64_t timeout = 10000;

    /* Send packets every interval up to available window size */
    while ( next_seqnum < base + cwnd ) {

      /* Are we due to send an outgoing packet right now? */
      const uint64_t now = timestamp();
      uint64_t next_packet_is_due = last_datagram_sent_ms + interval_ms;

      if ( now >= next_packet_is_due ) {
	/* Send a datagram */
	Packet send_packet( addr , next_seqnum, 0, "Hello from Client" );
	sock.send( send_packet );

	cout << "Sent packet with seqnum " << send_packet.sequence_number();
	cout << " at time " << send_packet.send_timestamp();
	cout << " to " << send_packet.addr().str() << endl; 
	
	last_datagram_sent_ms = now;
	next_packet_is_due = last_datagram_sent_ms + interval_ms;
	next_seqnum++;
      }
    }

    /* Wait for an event, and run callback if one comes */
    auto poll_result = poller.poll( timeout );

    if ( poll_result.result == Poller::Result::Type::Timeout ) {
      cout << "Timed out." << endl;
      //cout << "Cwnd was: " << cwnd << endl;
      ssthresh = max( 2, cwnd/2 );
      cwnd = 1;
      //cout << "Sshthresh now: " << ssthresh << endl;

      /* On loss, just send next packet */
      base = next_seqnum;

    }
    else if ( poll_result.result == Poller::Result::Type::Exit ) {
      /* An action wanted to quit or they all stopped being interested in their events */
      cout << "Quitting after a file descriptor received an error." << endl;
      return poll_result.exit_status;
    }
  }

  return EXIT_SUCCESS;
}

int main( int argc, char *argv[] ) {
  try { 
    /* Truly paranoid check */
    if ( argc <= 0 ) {
      throw Exception( "client", "Missing argv[ 0 ]" );
    }
    /* Check arguments */
    if ( argc != 3 ){
      throw Exception( argv[0] , "DEST_ADDRESS DEST_SERVICE" );
    }
    Client clt( argv[1], argv[2] );
    return clt.run();
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
