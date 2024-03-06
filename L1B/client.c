#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORTAS 20000

int main() 
{
    int client_socket;
    client_socket = socket(AF_INET6, SOCK_STREAM, 0);  
    if (client_socket == -1) {
        printf("Error creating socket.\n");
        return 1;
    }

    struct sockaddr_in6 server_address;
    server_address.sin6_family = AF_INET6;
    server_address.sin6_port = htons(PORTAS);
    inet_pton(AF_INET6, "::1", &(server_address.sin6_addr));

    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        printf("Error connecting to the server.\n");
        return 1;
    }

    printf("Successfully connected to the server.\n");

    char client_message[256];
    memset(client_message, '\0', sizeof(client_message));

    printf("Input your message: ");
    scanf("%255s", client_message);

    send(client_socket, client_message, strlen(client_message), 0);

    char server_response[1024];
    memset(server_response, '\0', sizeof(server_response));
    recv(client_socket, server_response, sizeof(server_response), 0);

    printf("Server response: %s\n", server_response);

    close(client_socket);

    return 0;
}