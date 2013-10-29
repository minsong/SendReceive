AM_CXXFLAGS = $(PICKY_CXXFLAGS)

bin_PROGRAMS = server client 
server_SOURCES = exception.hh ezio.cc ezio.hh \
	socket.cc socket.hh address.cc address.hh \
	socket_type.hh server.cc server.hh
server_LDADD = -lrt
server_LDFLAGS = -pthread
client_SOURCES = exception.hh ezio.cc ezio.hh \
	socket.cc socket.hh address.cc address.hh \
	socket_type.hh client.cc client.hh
client_LDADD = -lrt
client_LDFLAGS = -pthread
