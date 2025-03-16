#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <arpa/inet.h>
#include <unistd.h>

int get_socket(const char* address, struct addrinfo** results, char* port) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (address == NULL) {
        hints.ai_flags = AI_PASSIVE;
    }


    int getaddrinfo_res = getaddrinfo(address, port, &hints, results);
    if (getaddrinfo_res != 0) {
        const char* errmsg = gai_strerror(getaddrinfo_res);
        printf("%s\n", errmsg);
        return getaddrinfo_res;
    }

    int socket_fd = socket((*results)->ai_family, (*results)->ai_socktype, (*results)->ai_protocol);
    if (socket_fd < 0) {
        const char* error = strerror(errno);
        printf("%s\n", error);
    }

    return socket_fd;
}

int main() {
    struct addrinfo* addr;
    
    int socket_fd = get_socket(NULL, &addr, "5001");

    int connectResult = connect(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (connectResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return connectResult;
    }

    int buffer_size = 1024;
    char buffer[buffer_size];
    
    int bytes_received =  recv(socket_fd, buffer, buffer_size, 0);
    if (bytes_received < 0) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bytes_received;
    }

    printf("received bytes: %n\n", &bytes_received);
    printf("received msg: %s\n", buffer);

    close(socket_fd);

    return 0;
}
