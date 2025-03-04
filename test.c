#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <arpa/inet.h>

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
    
    int fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (fd == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return getaddrinfoResult;
    }

    int bindResult = bind(fd, results->ai_addr, results->ai_addrlen);
    if (bindResult == -1) {
        const char* error = strerror(errno);
        printf("%s\n", error);
        return getaddrinfoResult;
    }
    
    return 0;
}

int main() {
    human_to_machine();
    machine_to_human();
    make_dns_lookup();
    socket_test();
}
