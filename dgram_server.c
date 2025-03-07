#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <arpa/inet.h>

int get_socket(const char* address, struct addrinfo** results, char* port) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

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

    int bind_result = bind(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (bind_result == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bind_result;
    }

    int buffer_size = 1024;
    char buffer[buffer_size];
    
    uint ai_addr_size = sizeof(struct sockaddr_storage);
    int bytes_received =  recvfrom(socket_fd, buffer, buffer_size, 0, addr->ai_addr, &ai_addr_size);
    if (bytes_received < 0) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bytes_received;
    }

    printf("received bytes: %n\n", &bytes_received);
    printf("received msg: %s\n", buffer);

    return 0;
}
