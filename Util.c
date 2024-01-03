#include "Util.h"



int getIPv4TCPSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error : socket creation failed - ");
        exit(0);
    }
    return sockfd;
}

void getIPv4Address(const char *string, const int port, struct sockaddr_in *addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    int pton_ret = inet_pton(AF_INET, string, &(addr->sin_addr.s_addr));
    if (pton_ret == 0) {
        perror("Error : Cannot convert address Text to Binary - ");
        exit(0);
    } else if (pton_ret == -1) {
        perror("Error : convert address Text to Binary failed - ");
        exit(0);
    }
}

void getAnyIPv4Address(const int port, struct sockaddr_in *addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

