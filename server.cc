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
  : port(portNumber), addr("0.0.0.0", portNumber), sock(UDP)
{
  sock.bind(addr);
  cout << "Server port number: " << port << endl;
}

void Server::run( void ){
  while (true) {
    pair<Address, string> rcvd = sock.recvfrom();
    Packet received_packet( rcvd.first, rcvd.second );
    
    cout << "Server received message '" << received_packet.payload();
    cout << "' sent at " << received_packet.send_timestamp();
    cout << " from " << received_packet.addr().str() << endl;
    
    Packet send_packet( received_packet.addr(), 0 , 0, "ACK" );
    sock.sendto( send_packet.addr(), send_packet.str() );
  } 
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: server <port>" << endl;
    return EXIT_FAILURE;
  }
  try { 
    Server svr(myatoi(argv[1]));
    svr.run();
  } catch (const Exception & e) {
    e.perror();
    return EXIT_FAILURE;
  }
}
