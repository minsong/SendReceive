AM_CPPFLAGS = -I../protobufs
AM_CXXFLAGS = $(PICKY_CXXFLAGS)
LDADD = ../protobufs/libremyprotos.a -ljemalloc -lm $(protobuf_LIBS)

bin_PROGRAMS = server client

common_source = exception.hh ezio.cc ezio.hh \
	socket.cc socket.hh address.cc address.hh \
	socket_type.hh timestamp.cc timestamp.hh \
	packet.cc packet.hh int64.cc int64.hh

server_SOURCES = $(common_source) server.cc server.hh
server_LDADD = -lrt
server_LDFLAGS = -pthread
client_SOURCES = $(common_source) client.cc client.hh \
	poller.cc poller.hh memory.cc memory.hh \
	memoryrange.cc memoryrange.hh ../protobufs.dna.pb.h \
	whisker.cc whisker.hh whiskertree.cc whiskertree.hh \
	configrange.hh configrange.cc utility.hh
        # rat.cc rat.hh rat-templates.cc \
client_LDADD = -lrt
client_LDFLAGS = -pthread
