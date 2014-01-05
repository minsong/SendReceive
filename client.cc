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
  Packet send_packet( addr , 0 , 0, "Hello from Client" );
  
  /* Set up the events that we care about */
  Poller poller;

  while ( true ) {
    //send_packet.set_timestamp();
    //sock.sendto( send_packet.addr(), send_packet.str() );
    sock.send( send_packet );

    Packet received_packet = sock.recv();
    
    cout << "Client received message '" << received_packet.payload();
    cout << "' from " << received_packet.addr().str() << endl;
    
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
