#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORTAS 20000

void *receive_message(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char server_reply[256];
    while(1) {
        memset(server_reply, 0, sizeof(server_reply));
        if(recv(sock, server_reply, sizeof(server_reply), 0) > 0) {
            printf("Client2: %s\n", server_reply);
        }
    }
    return 0;
}

int main() {
    int sock;
    struct sockaddr_in6 server;
    pthread_t recv_thread;

    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("Could not create socket");
    }

    server.sin6_family = AF_INET6;
    server.sin6_port = htons(PORTAS);
    server.sin6_addr = in6addr_any;

    if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected\n");

    pthread_create(&recv_thread, NULL, receive_message, (void*) &sock);

    while(1) {
        char message[256];
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';
        if(send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }
    }

    close(sock);
    return 0;
}