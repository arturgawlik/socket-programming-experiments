#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <stdlib.h>

/*
    converts IPv4 address from human format to machine format
*/
int human_to_machine() {
    const char* ip_address_text_format = "192.168.0.1";
    struct sockaddr_in sa;

    if(inet_pton(AF_INET, ip_address_text_format, &(sa.sin_addr)) == -1) {
        return 1;
    }

    return 0;
}

/*
    converts IPv4 address machine format to human format 
*/
int machine_to_human() {
    char ipv4[INET_ADDRSTRLEN];
    struct sockaddr_in sa;

    inet_ntop(AF_INET, &sa.sin_addr, ipv4, INET_ADDRSTRLEN);

    return 0;
}

/*
    makes DNS lookup through `getaddrinfo` syscall
    and then prints all results in human readable format through `inet_ntop` syscall
*/
int make_dns_lookup() {
    char *address_to_lookup = "www.google.com"; 
    
    struct addrinfo hints;
    struct addrinfo *results;

    memset(&hints, 0, sizeof hints); // cleanups memory

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // my IP address

    int result = getaddrinfo(address_to_lookup, "https", &hints, &results);
    if (result != 0) {
        const char* error = gai_strerror(result);
        printf("%s\n", error);
        return result;
    }

    // iterate over all found addresses and print them
    struct addrinfo *next_sockaddr = results;
    while (next_sockaddr) {
        struct sockaddr *sockaddr = results->ai_addr;
        char *printAddressType;
        void *addr;

        if (next_sockaddr->ai_family == AF_INET) { // IPv4
            printAddressType = "IPv4";
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)sockaddr;
            addr = &(ipv4->sin_addr);
        }

        if (next_sockaddr->ai_family == AF_INET6) { // IPv6
            printAddressType = "IPv6";
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)sockaddr;
            addr = &(ipv6->sin6_addr);
        }
        
        char ip_human[INET6_ADDRSTRLEN];
        inet_ntop(next_sockaddr->ai_family, addr, ip_human, INET6_ADDRSTRLEN);
        printf("%s: %s\n", printAddressType, ip_human);

        next_sockaddr = next_sockaddr->ai_next;
    }

    freeaddrinfo(results); // deallocate memory

    return result;
}

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

int socket_test() {
    struct addrinfo hints;
    struct addrinfo *results;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // my IP

    int getaddrinfoResult = getaddrinfo(NULL, "3490", &hints, &results);
    if (getaddrinfoResult != 0) {
        const char* error = gai_strerror(getaddrinfoResult);
        printf("%s\n", error);
        return getaddrinfoResult;
    }
    
    int socketfd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (socketfd == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return socketfd;
    }

    int bindResult = bind(socketfd, results->ai_addr, results->ai_addrlen);
    if (bindResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bindResult;
    }

    close(socketfd);

    return 0;
}

int connect_test() {
    struct addrinfo* addr;
    
    int socket_fd = get_socket("www.google.com", &addr, "80");

    int connectResult = connect(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (connectResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return connectResult;
    }

    close(socket_fd);

    return 0;
}

int listen_test() {
    struct addrinfo* addr;
    
    int socket_fd = get_socket(NULL, &addr, "5000");

    int bind_result = bind(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (bind_result) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bind_result;
    }

    int listenResult = listen(socket_fd, 10);
    if (listenResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return listenResult;
    }

    close(socket_fd);

    return 0;
}

int accept_test() {
    struct addrinfo* addr;
    
    int socket_fd = get_socket(NULL, &addr, "5001");

    int bind_result = bind(socket_fd, addr->ai_addr, addr->ai_addrlen);
    if (bind_result == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return bind_result;
    }

    int listen_result = listen(socket_fd, 1000);
    if (listen_result == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return listen_result;
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
    send(connected_fd, msg_to_send, strlen(msg_to_send), 0);

    struct sockaddr_storage their_addr_from_peer;
    unsigned int sockaddrlen = (unsigned int)sizeof their_addr_from_peer; 
    int getpeername_result = getpeername(connected_fd, (struct sockaddr*)&their_addr_from_peer, &sockaddrlen);
    if (getpeername_result == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return getpeername_result;
    }

    char dst[100];
    void * ptr = inet_ntop(AF_INET, &their_addr_from_peer, dst, 100);
    if (ptr == NULL) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return 1;
    }

    // getnameinfo();
    // gethostbyaddr()    

    // char hostname[100];
    int max_hostname_length = 100;
    char* hostname = calloc(sizeof(char*), max_hostname_length);
    int res = gethostname(hostname,max_hostname_length);

    close(connected_fd);
    close(socket_fd);

    free(hostname);

    return 0;
}

int main() {
    // human_to_machine();
    // machine_to_human();
    // make_dns_lookup();
    // socket_test();
    // connect_test();
    // listen_test();
    accept_test();
}
