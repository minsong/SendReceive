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

Server::Server( const string s_local_service )
  : _addr( "0", s_local_service, UDP ), _sock( UDP )
{
  _sock.bind( _addr );
  cout << "Server port number: " << _addr.port() << endl;
}

int Server::run( void ){
  while ( true ) {
    Packet received_packet = _sock.recv();
 
    cout << "Server received '" << received_packet.payload();
    cout << "' with seqnum " << received_packet.sequence_number();
    cout << " at " << timestamp();
    cout << " sent at " << received_packet.send_timestamp();
    cout << " from " << received_packet.addr().str() << endl;
    
    //usleep(750000);
    //usleep(750000);
    
    uint16_t acknum = received_packet.sequence_number()+1;
    Packet send_packet( received_packet.addr(), 0, 0 , acknum, "ACK" );
    send_packet.set_echo_reply_timestamp( received_packet.send_timestamp() );
    _sock.send( send_packet );
    cout << "Sent packet with acknum " << send_packet.ack_number();
    cout << " at time " << send_packet.send_timestamp() << " to " << send_packet.addr().str() << endl; 
  } 

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  try { 
    if ( argc <= 0 ) {
      throw Exception( "server", "Missing argv[ 0 ]" );
    }
    if ( argc != 2 ) {
      throw Exception( argv[0], "LOCAL_SERVICE" );
    }
    Server svr( argv[ 1 ] );
    return svr.run();
  } catch ( const Exception & e ) {
    e.perror();
    return EXIT_FAILURE;
  }
}
