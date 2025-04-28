#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <unistd.h>

int get_socket(const char *address, struct addrinfo **results, char *port) {
  struct addrinfo hints;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (address == NULL) {
    hints.ai_flags = AI_PASSIVE;
  }

  int getaddrinfo_res = getaddrinfo(address, port, &hints, results);
  if (getaddrinfo_res != 0) {
    const char *errmsg = gai_strerror(getaddrinfo_res);
    printf("%s\n", errmsg);
    return getaddrinfo_res;
  }

  int socket_fd = socket((*results)->ai_family, (*results)->ai_socktype,
                         (*results)->ai_protocol);
  if (socket_fd < 0) {
    const char *error = strerror(errno);
    printf("%s\n", error);
  }

  return socket_fd;
}

int send_initial_http_data(int socket_fd) {
  const char *msg_to_send = "GET / HTTP/1.1\r\n\r\n";
  int msg_to_send_len = strlen(msg_to_send);
  int bytes_send = send(socket_fd, msg_to_send, msg_to_send_len, 0);
  if (bytes_send < 0) {
    const char *error = strerror(errno);
    printf("%s\n", error);
    return bytes_send;
  }
  if (bytes_send != msg_to_send_len) {
    printf("not all bytes were send by 'send' syscall. Bytes send to send: %n "
           "and bytes actually send: %n \n",
           &bytes_send, &msg_to_send_len);
    return 0; // error
  }

  return 0; // error
}

int main() {
  struct addrinfo *addr;

  int socket_fd = get_socket("www.google.com", &addr, "80");

  int connectResult = connect(socket_fd, addr->ai_addr, addr->ai_addrlen);
  if (connectResult == -1) {
    const char *error = strerror(errno);
    printf("%s\n", error);
    return connectResult;
  }

  int bytes_send = send_initial_http_data(socket_fd);
  if (bytes_send < 0) {
    return 1;
  }

  int buffer_size = 1024;
  char chunk_buffer[buffer_size];

  int bytes_received;
  /*
    If there is "0\r\n\r\n" at the end of response it indicates that this is
    last chunk of response.
    ("\0" - null terminator is added to properly count string length by
    `strlen`)
  */
  char *end_response_indicator = "0\r\n\r\n\0";
  int end_response_indicator_length = strlen(end_response_indicator);
  int end = 0;
  do {
    bytes_received = recv(socket_fd, chunk_buffer, buffer_size, 0);
    if (bytes_received < 0) {
      const char *error = strerror(errno);
      printf("%s\n", error);
      return bytes_received;
    }

    printf("%s", chunk_buffer);

    end = memmem(chunk_buffer, buffer_size, end_response_indicator,
                 end_response_indicator_length) != NULL;
  } while (!end);

  close(socket_fd);

  return 0;
}
