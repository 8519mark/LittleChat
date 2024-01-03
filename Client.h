#ifndef Client_h
#define Client_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>



typedef struct Client {
    int sockfd;
    volatile int stat;
    pthread_t tid;
    char name[16];
    struct sockaddr_in addr;
    char buf[1024];
} Client_t;

void client_connect(Client_t *client);
void client_send(const Client_t *client, const char *msg);
int client_read(Client_t *client);


#endif