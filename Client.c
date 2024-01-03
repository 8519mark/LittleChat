#include "Client.h"
#include "Util.h"

// Test - Google Client
// const char *SERVER_IP = "142.250.188.46";
// const int SERVER_PORT = 80;


const char *SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 6969;

const int BUFFER_LEN = 1024;
const int MAX_T_TILL_DISCONNECT = 3;

void client_connect(Client_t *client) {
    int conn_ret = connect(client->sockfd, (struct sockaddr *)&client->addr, sizeof(client->addr));
    if (conn_ret == -1) {
        close(client->sockfd);
        perror("Error : Connection to Server failed - ");
        exit(0);
    }
}

void client_send(const Client_t *client, const char *msg) {
    char *final_msg = (char *)malloc(strlen(client->name) + strlen(" : ") + strlen(msg) + 1);
    strcpy(final_msg, client->name);
    strcat(final_msg, " : ");
    strcat(final_msg, msg);
    int w_sent = send(client->sockfd, final_msg, strlen(final_msg), 0);
    if (w_sent == -1) {
        close(client->sockfd);
        free(final_msg);
        perror("Error : sending message fail - ");
        exit(0);
    } else if (w_sent != strlen(final_msg)) {
        printf("Message sent incomplete, continue ...\n");
    }
    free(final_msg);
}

int client_read(Client_t *client) {
    int r_read = recv(client->sockfd, client->buf, sizeof(client->buf), 0);
    if (r_read == -1) {
        return -1;
    }
    if (r_read == 0) {
        return 0;
    }
    client->buf[r_read] = '\0';
    printf("%s\n", client->buf);
    return r_read;
}

void *client_read_loop_thread(void *arg) {
    Client_t *client = (Client_t *)arg;
    int t = 0;
    while (client->stat == 1) {
        int read_stat = client_read(client);
        if (read_stat == -1) {
            close(client->sockfd);
            perror("Error : reading message fail - ");
            client->stat = 0;
        } else if (read_stat == 0) {
            t++;
            if (t == MAX_T_TILL_DISCONNECT) {
                printf("Connection Closed ...\n");
                client->stat = 0;
            }
        } else {
            t = 0;
        }
    }
    pthread_exit(NULL);
}

void client_send_loop(Client_t *client) {
    char *line = NULL;
    size_t len = 0;

    printf("Enter a message or 'exit' to end : \n");
    while (client->stat == 1) {
        ssize_t read = getline(&line, &len, stdin);

        if (read >= 0) {
            // remove newline, replaced with NULL terminator
            if (line[read - 1] == '\n') {
                line[read - 1] = '\0';
            }

            if (strcmp(line, "exit") == 0) {
                printf("Exiting the chat...\n");
                client->stat = 0;
            }
            client_send(client, line);
        } else {
            free(line);
            perror("Error : error in messgae loop - ");
            client->stat = 0;
        }
    }
    free(line);
}


int main() {
    Client_t client;
    client.stat = 1;

    char *line = NULL;
    size_t len = 0;
    printf("Enter user name : \n");
    ssize_t read = getline(&line, &len, stdin);
    if (read >= 0) {
        // remove newline, replaced with NULL terminator
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
    } else {
        free(line);
        perror("Error : error in messgae loop - ");
        exit(0);
    }
    strcpy(client.name, line);
    free(line);


    getIPv4Address(SERVER_IP, SERVER_PORT, &client.addr);
    client.sockfd = getIPv4TCPSocket();

    client_connect(&client);

    if (pthread_create(&client.tid, NULL, client_read_loop_thread, (void *)&client) != 0) {
        perror("Error : pthread_create ");
        exit(0);
    }
    client_send_loop(&client);

    pthread_join(client.tid, NULL);
    close(client.sockfd);

    return 0;
}