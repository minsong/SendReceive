#include <iostream>

#include "socket.hh"
#include "client.hh"
#include "packet.hh"

using namespace std;

Client::Client( const string ipAddress, const uint16_t portNumber )
  : port(portNumber), addr(ipAddress, portNumber), sock(UDP)
{
  cout << "-- New Client --" << endl;
}

void Client::run( void ){
  Packet send_packet( addr , 0 , 0, "Hello from Client" );
  
  while ( true ) {
    //send_packet.set_timestamp();
    //sock.sendto( send_packet.addr(), send_packet.str() );
    sock.send(send_packet);

    Packet received_packet = sock.recv();
    
    cout << "Client received message '" << received_packet.payload();
    cout << "' from " << received_packet.addr().str() << endl;
    
    usleep(999999);
  } 
}

int main( int argc, char *argv[] ) {
  if (argc != 3){
    cerr << "Usage: client <ip address> <port>" << endl;
    return EXIT_FAILURE;
  }
  try { 
    Client clt(argv[1],myatoi(argv[2]));
    clt.run();
  } catch (const Exception & e) {
    e.perror();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
