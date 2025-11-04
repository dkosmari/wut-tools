#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define SERVER_PORT 4405

using std::cerr;
using std::cout;
using std::endl;
using std::flush;

volatile std::sig_atomic_t interrupted;

extern "C"
void handle_ctrl_c(int)
{
   interrupted = 1;
}

int main(int argc, char **argv)
{
   struct sockaddr_in addr;
   unsigned short port = SERVER_PORT;

   if (argc == 2) {
      port = atoi(argv[1]);
   }

#ifdef _WIN32
   WSADATA wsaData;
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
      cerr << "WSAStartup() failed" << endl;
      return -1;
   }
#endif

   // Create socket
   auto fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
   if (fd == INVALID_SOCKET) {
      WSACleanup();
#else
   if (fd < 0) {
#endif
      cerr << "Failed to create socket" << endl;
      return -1;
   }

   // Bind socket
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(port);
   if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
#ifdef _WIN32
      closesocket(fd);
      WSACleanup();
#else
      close(fd);
#endif
      cerr << "Failed to bind socket" << endl;
      return -1;
   }

   // Receive data
   char buffer[2048];
   bool running = true;

   std::signal(SIGINT, handle_ctrl_c);

   while (running) {

      fd_set fdsRead;
      FD_ZERO(&fdsRead);
      FD_SET(fd, &fdsRead);

      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 250'000;

      if (select(fd + 1, &fdsRead, NULL, NULL, &tv) == 1) {
         struct sockaddr_in from;
#ifdef _WIN32
         int fromLen = sizeof(from);
#else
         socklen_t fromLen = sizeof(from);
#endif
         int recvd = recvfrom(fd,
                              buffer,
                              sizeof(buffer),
                              0,
                              reinterpret_cast<struct sockaddr *>(&from),
                              &fromLen);

         if (recvd > 0)
            cout << std::string_view{buffer, static_cast<std::size_t>(recvd)} << flush;

      }

      if (interrupted) {
         cerr << "\nInterrupted." << endl;
         running = false;
      }
   }

#ifdef _WIN32
   closesocket(fd);
   WSACleanup();
#else
   close(fd);
#endif
   return 0;
}
