#include <iostream>
#include <limits>

#include "client.hh"
#include "poller.hh"
#include "timestamp.hh"

using namespace std;
using namespace PollerShortNames;

static const double alpha = 1.0 / 8.0;
static const double beta = 1.0 / 4.0;

const uint64_t K = 4;

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

  uint64_t ssthresh = numeric_limits<int>::max();
  uint64_t cwnd = 1;
  uint64_t ca_incr = 0; //congestion avoidance counter used to increment cwnd once every RTT
  uint64_t base = 0;
  uint64_t next_seqnum = 0; 

  /* Initial RTO = 1 second  */
  uint64_t rto = 1000;
  uint64_t srtt, rttvar;
  

 /* Set up the events that we care about */
  Poller poller;

  /*Receive a packet */
  poller.add_action( Poller::Action( sock.fd(),
				     Direction::In,
				     [&] () {
				       const auto received_packet = sock.recv();
				       
				       update_timeout( received_packet, srtt, rttvar, rto );
				       update_window_size( ssthresh, ca_incr, cwnd );
				       base = received_packet.ack_number();

				       cout << "Received '" << received_packet.payload();
				       cout << "' with acknum " << received_packet.ack_number();
				       cout << " from " << received_packet.addr().str() << endl;
				       
				       return ResultType::Continue;
				     } ) );

  while ( true ) {
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
    auto poll_result = poller.poll( rto );

    if ( poll_result.result == Poller::Result::Type::Timeout ) {
      cout << "Timed out." << endl;
      //cout << "Cwnd was: " << cwnd << endl;
      ssthresh = max( ( uint64_t ) 2, cwnd/2 );
      cwnd = 1;

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

void Client::update_timeout( const Packet &received_packet, uint64_t &srtt, uint64_t &rttvar, uint64_t &rto ){
  /* Get sample round trip time */
  const uint64_t received_time = timestamp();
  const uint64_t r = received_time - received_packet.echo_reply_timestamp();
  //cout << "Received Time: " << received_time << endl;
  //cout << "RTT sample: " << r << endl;

  /* Get smoothed round trip time estimate and variance */
  if ( !(srtt) ) {
    srtt = r;
    rttvar = r/2;
  }
  else {
    const uint64_t delta = srtt > r ? srtt-r : r-srtt;
    rttvar = (1 - beta)*rttvar + beta*delta;
    srtt = (1-alpha)*srtt + alpha*r;
  }

  /* Get retransmission timeout */
  // TODO Clock granularity: rto = srtt + max( G, K*rttvar ); 
  rto = max( srtt + K*rttvar, ( uint64_t ) 1000 );
  //cout << "SRTT: " << srtt << " RTTVAR: " << rttvar << endl;
  //cout << "RTO: " << rto << endl;
}

void Client::update_window_size( const uint64_t ssthresh, uint64_t &ca_incr, uint64_t &cwnd ){
  /* Slow start */ 
  if (cwnd < ssthresh){
    cwnd++;
    cout << "Slow start-> incremented cwnd: " << cwnd << endl;
  }
  /* Congestion avoidance: cwnd >= ssthresh */
  else {
    /* Count up to cwnd, then increment cwnd */
    if ( ++ca_incr % cwnd  == 0 ){
      cwnd++;
      ca_incr = 0;
      cout << "Congestion avoidance-> incremented cwnd: " << cwnd << endl;
    }
  }
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
