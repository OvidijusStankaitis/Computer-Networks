#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORTAS 20000

int main() 
{
    int server_socket;
    server_socket = socket(AF_INET6, SOCK_STREAM, 0);  

    if (server_socket == -1) {
        printf("Socket creation failed\n");
        return 1;
    }

    struct sockaddr_in6 server_address;
    server_address.sin6_family = AF_INET6;
    server_address.sin6_port = htons(PORTAS);
    inet_pton(AF_INET6, "::1", &(server_address.sin6_addr));

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        printf("Binding failed\n");
        return 1;
    }

    if (listen(server_socket, 1) == -1) {
        printf("Listening failed\n");
        return 1;
    }

    printf("Server is up and running. Waiting for incoming messages...\n");
    while (1) {
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);

        if (client_socket == -1) {
            printf("Accepting client connection failed\n");
            return 1;
        }

        char client_message[256];
        memset(client_message, 0, sizeof(client_message));
        recv(client_socket, client_message, sizeof(client_message), 0);

        printf("Received message: %s\n", client_message);

        if (strcmp(client_message, "exit") == 0) {
            printf("Server is powering down\n");
            close(client_socket);
            close(server_socket);
            return 0;
        }

        for (size_t i = 0; i < strlen(client_message); i++) {
            client_message[i] = toupper(client_message[i]);
        }

        char modified_message[512];
        memset(modified_message, 0, sizeof(modified_message));
        sprintf(modified_message, "%s", client_message);

        send(client_socket, modified_message, sizeof(modified_message), 0);

        printf("Sent message: %s\n", modified_message);
    }
}