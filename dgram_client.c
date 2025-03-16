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
    
    const char* msg_to_send = "hello from the dgram side.";
    int msg_to_send_len = strlen(msg_to_send);
    uint ai_addr_size = sizeof(struct sockaddr_storage);
    int bytes_send = sendto(socket_fd, msg_to_send, msg_to_send_len, 0, addr->ai_addr, ai_addr_size);
    if (bytes_send == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bytes_send;
    }
    if (bytes_send != msg_to_send_len) {
        printf("not all bytes were send by 'send' syscall. Bytes send to send: %n and bytes actually send: %n \n", &bytes_send, &msg_to_send_len);
    }

    close(socket_fd);

    return 0;
}
