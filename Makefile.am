AM_CXXFLAGS = $(PICKY_CXXFLAGS)

bin_PROGRAMS = receiver sender 
receiver_SOURCES = exception.hh ezio.cc ezio.hh \
	socket.cc socket.hh address.cc address.hh \
	socket_type.hh receiver.cc
receiver_LDADD = -lrt
receiver_LDFLAGS = -pthread
sender_SOURCES = exception.hh ezio.cc ezio.hh \
	socket.cc socket.hh address.cc address.hh \
	socket_type.hh sender.cc
sender_LDADD = -lrt
sender_LDFLAGS = -pthread

install-exec-hook:
	chown root $(DESTDIR)$(bindir)/receiver
	chmod u+s $(DESTDIR)$(bindir)/receiver
	chown root $(DESTDIR)$(bindir)/sender
	chmod u+s $(DESTDIR)$(bindir)/sender
