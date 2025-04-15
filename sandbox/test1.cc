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
  int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd < 0) {
    const char *error = strerror(errno);
    printf("%s\n", error);
  }

  printf("all good\n");
}
