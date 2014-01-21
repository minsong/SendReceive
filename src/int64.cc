#define _BSD_SOURCE
#include <endian.h>

#include "int64.hh"

using namespace std;

/* Construct integer from network-byte-order string */
Integer64::Integer64( const string & contents )
  : contents_( 0 )
{
  if ( contents.size() != sizeof( uint64_t ) ) {
    throw string( "Size mismatch in Integer64 constructor." );
  }

  contents_ = be64toh( *(uint64_t *)contents.data() );
}

/* Produce network-byte-order representation of integer */
string Integer64::str( void ) const
{
  uint64_t network_order = htobe64( contents_ );
  return string( (char *)&network_order, sizeof( uint64_t ) );
}
