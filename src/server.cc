#include <iostream>
#include <stdlib.h>
#include <string>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"
#include "server.hh"
#include "packet.hh"
#include "timestamp.hh"

using namespace std;

Server::Server( const string local_service )
  : addr( "0", local_service, UDP ), sock( UDP )
{
  sock.bind( addr );
  cout << "Server port number: " << addr.port() << endl;
}

int Server::run( void ){
  while ( true ) {
    Packet received_packet = sock.recv();
 
    cout << "Server received '" << received_packet.payload();
    cout << "' with seqnum " << received_packet.sequence_number();
    cout << " at " << timestamp();
    cout << " sent at " << received_packet.send_timestamp();
    cout << " from " << received_packet.addr().str() << endl;
    
    /* Used for testing RTT samples */
    usleep(750000);
    usleep(750000);
    
    uint16_t acknum = received_packet.sequence_number()+1;
    Packet send_packet( received_packet.addr(), 0, 0 , acknum, "ACK" );
    send_packet.set_echo_reply_timestamp( received_packet.send_timestamp() );
    sock.send( send_packet );
    cout << "Sent packet with acknum " << send_packet.ack_number();
    //cout << " with echoed timestamp " << send_packet.echo_reply_timestamp();
    cout << " at time " << send_packet.send_timestamp() << " To " << send_packet.addr().str() << endl; 
  } 

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  try { 
    /* Truly paranoid check */
    if ( argc <= 0 ) {
      throw Exception( "server", "Missing argv[ 0 ]" );
    }
    /* Check arguments */
    if ( argc != 2 ) {
      throw Exception( argv[0], "LOCAL_SERVICE" );
    }
    Server svr( argv[1] );
    return svr.run();
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
