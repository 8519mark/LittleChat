#include "Server.h"
#include "Util.h"


const int SERVER_PORT = 6969;

const int BUFFER_LEN = 1024; 

const int NUMBER_OF_PENDING_CONNECTIONS = 10;



void server_bind(Server_t *server) {
    int bind_ret = bind(server->sockfd, (struct sockaddr *)&server->addr, sizeof(server->addr));
    if (bind_ret == -1) {
        close(server->sockfd);
        perror("Error : cannot bind socket ");
        exit(0);
    }
    int option = 1;
    setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}
void server_listen(Server_t *server) {
    int list_ret = listen(server->sockfd, NUMBER_OF_PENDING_CONNECTIONS);
    if (list_ret == -1) {
        close(server->sockfd);
        perror("Error : cannot listen on given socket ");
        exit(0);
    }
}



int server_accept(Server_t *server) {
    if (server->next_c_index >= NUMBER_OF_Clients) {
        return -1;
    }

    int i = server->next_c_index;
    int c_sock_fd = accept(server->sockfd, (struct sockaddr *)&server->clients[i].addr, &server->clients[i].addrlen);
    if (c_sock_fd == -1) {
        close(server->sockfd);
        perror("Error : cannot accept client ");
        exit(0);
    }
    server->clients[i].sockfd = c_sock_fd;
    return server->next_c_index++;
}

void *server_rw_loop(void *arg) {
    Server_to_Thread_t *s_t = (Server_to_Thread_t *)arg;
    Server_t *server = s_t->server_ptr;
    int cur_index = s_t->client_index;

    Client_t *client = &server->clients[cur_index];

    char r_buf[BUFFER_LEN];
    while(1) {
        int r_read = recv(client->sockfd, r_buf, sizeof(r_buf), 0);
        if (r_read == -1) {
            close(client->sockfd);
            printf("Error : reading message fail - ");
            break;
        } else if (r_read == 0) {
            printf("Connection Closed ...\n");
            break;
        } else if (r_read > 0) {
            r_buf[r_read] = '\0';
            printf("%s\n", r_buf);
            int w_sent = send(client->sockfd, " --- ack --- \n", strlen("ack\n"), 0);
            for (int i = 0; i < server->next_c_index; i++) {
                if (i != cur_index) {
                    send(server->clients[i].sockfd, r_buf, strlen(r_buf), 0);
                }
            }
        }
    }
    pthread_exit(NULL);
}

void server_loop(void *arg) {
    Server_t *server = (Server_t *)arg;

    while (1) {
        int i = server_accept(server);
        printf("tid check : %d\n", i);

        Server_to_Thread_t s_t;
        s_t.client_index = i;
        s_t.server_ptr = server;

        if (pthread_create(&server->tids[i], NULL, server_rw_loop, (void *)&s_t) != 0) {
            perror("Error: pthread_create ");
            exit(0);
        }
    }
}


void server_close(Server_t *server) {
    for (int i = 0; i < server->next_c_index; i++) {
        close(server->clients[i].sockfd);
    }
    shutdown(server->sockfd, SHUT_RDWR);
}



int main() {
    Server_t server;
    server.next_c_index = 0;
    getAnyIPv4Address(SERVER_PORT, &server.addr);
    server.sockfd = getIPv4TCPSocket();

    server_bind(&server);
    server_listen(&server);

    server_loop(&server);

    server_close(&server);
    
    return 0;
}
