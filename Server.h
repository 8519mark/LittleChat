#ifndef Server_h
#define Server_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>



#define NUMBER_OF_Clients 10

typedef struct Client {
    int sockfd;
    pthread_t tid;
    char name[16];
    struct sockaddr_in addr;
    socklen_t addrlen;
} Client_t;

typedef struct Server {
    int sockfd;
    struct sockaddr_in addr;
    int next_c_index;
    char buf[1024];

    pthread_t tids[NUMBER_OF_Clients];
    Client_t clients[NUMBER_OF_Clients];
} Server_t;


typedef struct Server_to_Thread {
    int client_index;
    Server_t *server_ptr;
} Server_to_Thread_t;


void server_bind(Server_t *server);
void server_listen(Server_t *server);
// return index of the current client in server array, or -1 if it is full
int server_accept(Server_t *server);
void server_send(Client_t *client, const char *msg);
void server_read(Client_t *client);


#endif