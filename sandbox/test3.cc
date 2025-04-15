// having fun with sockets
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
  int domain = AF_UNIX; // local communication
  int type = SOCK_RAW;  // going wild with raw communication
  int protocol = 0;     // inheritted default from domain

  int socket_fd = socket(domain, type, protocol);
  if (socket_fd < 0) {
    const char *error = strerror(errno);
    printf("%s\n", error);
  }

  printf("program run\n");
}
