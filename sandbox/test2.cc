// having fun with getting IPs of services

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>

int main(int argc, char **argv) {
  //   const char *host = "www.nodejs.org";
  const char *host = argv[1];
  const char *port = "https";
  struct addrinfo hints;
  struct addrinfo *results;
  memset(&hints, 0, sizeof(addrinfo));

  //   hints.ai_family = AF_UNSPEC;
  //   hints.ai_socktype = SOCK_STREAM;
  //   hints.ai_flags = AI_PASSIVE; // my IP

  int getaddrinfoResult = getaddrinfo(host, port, &hints, &results);
  if (getaddrinfoResult != 0) {
    const char *error = gai_strerror(getaddrinfoResult);
    printf("%s\n", error);
    return getaddrinfoResult;
  }

  struct addrinfo *next_sockaddr = results;
  while (next_sockaddr) {
    struct sockaddr *sockaddr = results->ai_addr;
    const char *printAddressType;
    void *addr;

    if (next_sockaddr->ai_family == AF_INET) { // IPv4
      printAddressType = "IPv4";
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)sockaddr;
      addr = &(ipv4->sin_addr);
    }

    if (next_sockaddr->ai_family == AF_INET6) { // IPv6
      printAddressType = "IPv6";
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)sockaddr;
      addr = &(ipv6->sin6_addr);
    }

    char ip_human[INET6_ADDRSTRLEN];
    inet_ntop(next_sockaddr->ai_family, addr, ip_human, INET6_ADDRSTRLEN);
    printf("%s: %s\n", printAddressType, ip_human);

    next_sockaddr = next_sockaddr->ai_next;
  }

  freeaddrinfo(results); // deallocate memory
}
