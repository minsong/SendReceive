#include <stdlib.h>
#include <iostream>
#include <cstring>     
#include <sys/socket.h>
#include <netdb.h>

#include "socket.hh"

const int BUFSIZE = 1024;
const std::string LOCALHOST = "127.0.0.1";

int main(int argc, char *argv[]) {

   if (argc != 2)
   {
        std::cerr << "Usage: receiver <port>" << std::endl;
        exit(1);
   }
   try
   {
        Socket sock(UDP); 
	Address rcvaddr(LOCALHOST, myatoi(argv[1]));
        sock.bind(rcvaddr);

        std::cout << "Waiting for data..." << std::endl;
        std::pair<Address, std::string> rcvd = sock.recvfrom();
        std::cout << "Sender: " << rcvd.first.str() << std::endl;
        std::cout << "Payload: " << rcvd.second << std::endl;
    }
    catch (const Exception & e)
    {
        e.perror();
	return EXIT_FAILURE;
    }

      return 0;
}
