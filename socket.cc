/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <sys/socket.h>
#include <netinet/in.h>
#include <utility>
#include <arpa/inet.h>
#include <cassert>

#include "socket.hh"
#include "exception.hh"
#include "address.hh"
#include "ezio.hh"
#include "packet.hh"

using namespace std;

Socket::Socket( const SocketType & socket_type )
    : fd_( socket( AF_INET, socket_type, 0 ), "socket" ),
      local_addr_(),
      peer_addr_()
{
}

Socket::Socket( FileDescriptor && s_fd, const Address & s_local_addr, const Address & s_peer_addr )
  : fd_( move( s_fd ) ),
    local_addr_( s_local_addr ),
    peer_addr_( s_peer_addr )
{
}

void Socket::bind( const Address & addr )
{
    /* make local address to listen on */
    local_addr_ = addr;
 
    /* bind the socket to listen_addr */
    if ( ::bind( fd_.num(),
                 &local_addr_.raw_sockaddr(),
                 sizeof( local_addr_.raw_sockaddr() ) ) < 0 ) {
        throw Exception( "bind" );
    }

    /* set local_addr to the address we actually were bound to */
    sockaddr_in new_local_addr;
    socklen_t new_local_addr_len = sizeof( new_local_addr );

    if ( ::getsockname( fd_.num(),
                        reinterpret_cast<sockaddr *>( &new_local_addr ),
                        &new_local_addr_len ) < 0 ) {
        throw Exception( "getsockname" );
    }

    local_addr_ = Address( new_local_addr );
}

static const int listen_backlog_ = 16;

void Socket::listen( void )
{
  if ( ::listen( fd_.num(), listen_backlog_ ) < 0 ) {
    throw Exception( "listen" );
  }
}

Socket Socket::accept( void )
{
  /* make new socket address for connection */
  sockaddr_in new_connection_addr;
  socklen_t new_connection_addr_size = sizeof( new_connection_addr );

  /* wait for client connection */
  FileDescriptor new_fd( ::accept( fd_.num(),
                         reinterpret_cast<sockaddr *>( &new_connection_addr ),
                         &new_connection_addr_size ), "accept" );

  // verify length is what we expected 
  if ( new_connection_addr_size != sizeof( new_connection_addr ) ) {
    throw Exception( "sockaddr size mismatch" );
  }
  
  return Socket( move( new_fd ), local_addr_, Address( new_connection_addr ) );
}

string Socket::read( void )
{
    return fd_.read();
}

void Socket::connect( const Address & addr )
{
    peer_addr_ = addr;

    if ( ::connect( fd_.num(),
                    &peer_addr_.raw_sockaddr(),
                    sizeof( peer_addr_.raw_sockaddr() ) ) < 0 ) {
        throw Exception( "connect" );
    }
}

void Socket::write( const std::string & str )
{
    fd_.write( str );
}

pair< Address, string > Socket::recvfrom( void )
{
    static const ssize_t RECEIVE_MTU = 2048;

    /* receive source address and payload */
    sockaddr_in packet_remote_addr;
    char buf[ RECEIVE_MTU ];

    socklen_t fromlen = sizeof( packet_remote_addr );

    ssize_t recv_len = ::recvfrom( fd_.num(),
                                   buf,
                                   sizeof( buf ),
                                   MSG_TRUNC,
                                   reinterpret_cast< sockaddr * >( &packet_remote_addr ),
                                   &fromlen );

    if ( recv_len < 0 ) {
        throw Exception( "recvfrom" );
    } else if ( recv_len > RECEIVE_MTU ) {
        throw Exception( "oversized datagram" );
    }

    return make_pair( Address( packet_remote_addr ),
                      string( buf, recv_len ) );
}

void Socket::sendto( const Address & destination, const std::string & payload )
{
    if ( ::sendto( fd_.num(),
                   payload.data(),
                   payload.size(),
                   0,
                   &destination.raw_sockaddr(),
                   sizeof( destination.raw_sockaddr() ) ) < 0 ) {
        throw Exception( "sendto" );
    }
}

/* Send packet */
void Socket::send( Packet & packet )
{
    packet.set_timestamp();
    string payload( packet.str() );
    
    ssize_t bytes_sent = ::sendto( fd_.num(), payload.data(),
                                   payload.size(), 0,
                                   (sockaddr *)&packet.addr().raw_sockaddr(),
                                   sizeof( packet.addr().raw_sockaddr() ) );
    
    if ( bytes_sent != static_cast<ssize_t>( payload.size() ) ) {
        throw Exception( "sendto error" );
  }
}

/* Receive a packet and associated timestamp */
Packet Socket::recv( void )
{
  static const int RECEIVE_MTU = 2048;

  /* receive source address, timestamp, and payload in msghdr structure */
  struct sockaddr_in packet_remote_addr;
  struct msghdr header;
  struct iovec msg_iovec;

  char msg_payload[ RECEIVE_MTU ];
  char msg_control[ RECEIVE_MTU ];

  /* receive source address */
  header.msg_name = &packet_remote_addr;
  header.msg_namelen = sizeof( packet_remote_addr );

  /* receive payload */
  msg_iovec.iov_base = msg_payload;
  msg_iovec.iov_len = RECEIVE_MTU;
  header.msg_iov = &msg_iovec;
  header.msg_iovlen = 1;

  /* receive timestamp */
  header.msg_control = msg_control;
  header.msg_controllen = RECEIVE_MTU;

  /* receive flags */
  header.msg_flags = 0;

  ssize_t received_len = recvmsg( fd_.num(), &header, 0 );

  if ( received_len < 0 ) {
    throw Exception( "recvmsg" );
  }

  if ( header.msg_flags & MSG_TRUNC ) {
    throw Exception( "Received oversize datagram" );
  }

  /*TODO: verify presence of timestamp */
  /* struct cmsghdr *ts_hdr = CMSG_FIRSTHDR( &header );
  assert( ts_hdr );
  assert( ts_hdr->cmsg_level == SOL_SOCKET );
  assert( ts_hdr->cmsg_type == SO_TIMESTAMPNS );*/

  return Packet( Address( packet_remote_addr ),
                 string( msg_payload, received_len ) );
}
