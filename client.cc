#include <iostream>

#include "socket.hh"
#include "client.hh"
#include "packet.hh"
#include "poller.hh"

using namespace std;
using namespace PollerShortNames;

Client::Client( const string ipAddress, const uint16_t portNumber )
  : port( portNumber ), addr( ipAddress, portNumber ), sock( UDP )
{
  cout << "-- New Client --" << endl;
}

void Client::run( void ){  
  /* Set up the events that we care about */
  Poller poller;

  uint16_t cwnd = 5;
  uint16_t base = 0;
  uint16_t next_seqnum = 0; 

  while ( true ) {
    //send_packet.set_timestamp();
    //sock.sendto( send_packet.addr(), send_packet.str() );

    cout << "--Base is " << base << "--" << endl;

    /* Send packets up to available window size */
    while ( next_seqnum < base + cwnd ) {
      Packet send_packet( addr , next_seqnum, 0, "Hello from Client" );

      cout << "-- Sending Packet with seqnum " << next_seqnum << "--" << endl; 
      sock.send( send_packet );
      
      next_seqnum++;
    }


    Packet received_packet = sock.recv();
    cout << "Client received message '" << received_packet.payload();
    cout << "' with acknum " << received_packet.ack_number();
    cout << " from " << received_packet.addr().str() << endl;
    
    base = received_packet.ack_number();

    usleep( 999999 );
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
      throw Exception( argv[0] , "DEST_ADDRESS DEST_PORT" );
      return EXIT_FAILURE;
    }
    Client clt( argv[1], myatoi( argv[2] ) );
    clt.run();
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
