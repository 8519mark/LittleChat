#ifndef Util_h
#define Util_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ifaddrs.h>



// get a file descriptor of the IPv4 TCP socket
int getIPv4TCPSocket();

// given IP string and port number, put the info in 'addr'
void getIPv4Address(const char *string, const int port, struct sockaddr_in *addr);
// given a port number, put the info in 'addr' --> any IP available on machine
void getAnyIPv4Address(const int port, struct sockaddr_in *addr);

#endif