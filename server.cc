#include <iostream>
#include <stdlib.h>
#include <string>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"
#include "server.hh"
#include "packet.hh"

const int BUFSIZE = 1024;

using namespace std;

Server::Server( const uint16_t portNumber )
  : port( portNumber ), addr( "0.0.0.0", portNumber ), sock( UDP )
{
  sock.bind( addr );
  cout << "Server port number: " << port << endl;
}

void Server::run( void ){
  while ( true ) {
    Packet received_packet = sock.recv();
    
    cout << "Server received message '" << received_packet.payload();
    cout << "' with seqnum " << received_packet.sequence_number();
    cout << " sent at " << received_packet.send_timestamp();
    cout << " from " << received_packet.addr().str() << endl;
    
    Packet send_packet( received_packet.addr(), 0 , received_packet.sequence_number()+1, "ACK" );
    sock.send( send_packet );
  } 
}

int main(int argc, char *argv[]) {
  try { 
    /* Truly paranoid check */
    if ( argc <= 0 ) {
      throw Exception( "server", "Missing argv[ 0 ]" );
    }
    /* Check arguments */
    if ( argc != 2 ) {
      throw Exception( argv[0], "LOCAL_PORT" );
      return EXIT_FAILURE;
    }
    Server svr( myatoi( argv[1] ) );
    svr.run();
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
