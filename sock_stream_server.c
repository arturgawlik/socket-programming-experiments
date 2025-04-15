#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>

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

    int bind_result = bind(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (bind_result == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bind_result;
    }

    int listenResult = listen(socket_fd, 1000);
    if (listenResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return listenResult;
    }

    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof their_addr;
    int connected_fd = accept(socket_fd, (struct sockaddr*)&their_addr, &addr_size);
    if (connected_fd == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return connected_fd;
    }

    const char* msg_to_send = "hello from the other side.";
    int msg_to_send_len = strlen(msg_to_send);
    int bytes_send = send(connected_fd, msg_to_send, msg_to_send_len, 0);
    if (bytes_send < 0) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bytes_send;
    }
    if (bytes_send != msg_to_send_len) {
        printf("not all bytes were send by 'send' syscall. Bytes send to send: %n and bytes actually send: %n \n", &bytes_send, &msg_to_send_len);
    }

    int close_res = close(connected_fd);
    if (close_res == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return close_res;
    }
    close_res = close(socket_fd);
    if (close_res == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return close_res;
    }

    return 0;
}
