#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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
#include <excmd.h>
#include <fmt/base.h>
#include <fmt/ostream.h>
#include <iostream>
#include <stdexcept>
#include <string_view>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define SERVER_PORT 4405

/*
 * Note: you can get the same functionality from (OpenBSD) netcat:
 *     nc -4 -l -u 4405
 */

using std::cerr;
using std::clog;
using std::cout;

using namespace std::literals;

volatile std::sig_atomic_t interrupted;

extern "C"
void handle_ctrl_c(int)
{
   interrupted = 1;
}

static std::string
to_string(const sockaddr_in& addr)
{
   char result[INET_ADDRSTRLEN];
   if (!inet_ntop(addr.sin_family, &addr.sin_addr, result, sizeof result))
      throw std::logic_error{"cannot print address"};
   return result + ":"s + std::to_string(ntohs(addr.sin_port));
}

static std::string
errno_to_string()
{
#ifdef _WIN32
   char *s = nullptr;
   FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                 | FORMAT_MESSAGE_FROM_SYSTEM,
                 nullptr,
                 WSAGetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 reinterpret_cast<LPTSTR>(&s),
                 0,
                 nullptr);
   std::string result = s;
   LocalFree(s);
   return result;
#else
   char result[256];
   return strerror_r(errno, result, sizeof result);
#endif
}

static void
show_help(std::ostream& out,
          const excmd::parser& parser,
          const std::string& exec_name)
{
   fmt::println(out, "Usage:");
   fmt::println(out, "  {} [options] [port]", exec_name);
   fmt::println(out, "{}", parser.format_help(exec_name));
   fmt::println(out, "Report bugs to {}", PACKAGE_BUGREPORT);
}

int main(int argc, char **argv)
{
   excmd::parser parser;
   excmd::option_state options;
   using excmd::description;
   using excmd::value;

   unsigned short port = SERVER_PORT;
   bool verbose = false;

   try {
      parser.global_options()
         .add_option("h,help",
                     description { "Show help" })
         .add_option("version",
                     description { "Show version" })
         .add_option("v,verbose",
                     description { "Print verbose messages to STDERR" })
         ;
      parser.default_command()
         .add_argument("port",
                       description { "Set listening port (default is "s
                                     + std::to_string(SERVER_PORT) + ")"s },
                       excmd::optional {},
                       value<int> {})
         ;

      options = parser.parse(argc, argv);
   }
   catch (std::exception& ex) {
      fmt::println(cerr, "Error parsing options: {}", ex.what());
      return -1;
   }

   if (options.has("help")) {
      show_help(cout, parser, argv[0]);
      return 0;
   }

   if (options.has("version")) {
      fmt::println(cout, "{} ({}) {}", argv[0], PACKAGE_NAME, PACKAGE_VERSION);
      return 0;
   }

   if (options.has("port"))
      port = options.get<int>("port");

   verbose = options.has("verbose");

#ifdef _WIN32
   WSADATA wsaData;
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
      fmt::println(cerr, "WSAStartup() failed");
      return -1;
   }
#endif

   // Create socket
   auto fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
#ifdef _WIN32
   if (fd == INVALID_SOCKET) {
      auto msg = errno_to_string();
      WSACleanup();
#else
   if (fd < 0) {
      auto msg = errno_to_string();
#endif
      fmt::println(cerr, "Failed to create socket: {}", msg);
      return -1;
   }
   if (verbose)
      fmt::println(clog, "Created socket {}", fd);

   // Bind socket
   struct sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_port = htons(port);
   if (verbose)
      fmt::println(clog, "Binding socket {} to {}", fd, to_string(addr));
   if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
#ifdef _WIN32
      auto msg = errno_to_string(); // grab error message before any other syscall
      closesocket(fd);
      WSACleanup();
#else
      auto msg = errno_to_string(); // grab error message before any other syscall
      close(fd);
#endif
      fmt::println(cerr, "Failed to bind socket: {}", msg);
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

         if (recvd > 0) {
            if (verbose)
               fmt::println(clog, "Received {} bytes.", recvd);
            fmt::println(cout, "{}", std::string_view{buffer, static_cast<std::size_t>(recvd)});
            cout.flush();
         } else {
            if (verbose) {
               auto msg = errno_to_string();
               fmt::println(clog, "recvfrom() returned {}: {}", recvd, msg);
            }
         }
      }

      if (interrupted) {
         if (verbose)
            fmt::println(clog, "\nInterrupted.");
         else
            fmt::println(clog, "");
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
