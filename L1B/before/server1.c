#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT1 20000
#define PORT2 21000
#define PORT3 22000

int client_sockets[2] = {-1, -1}; // Stores client sockets, initialized to -1

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    int read_size;
    char client_message[256];

    int socks;
    struct sockaddr_in6 server;

    if (client_sockets[0] == sock)
    {
        socks = socket(AF_INET6, SOCK_STREAM, 0);
        if (socks == -1)
        {
            printf("Could not create socket");
        }

        server.sin6_family = AF_INET6;
        server.sin6_port = htons(PORT3);
        server.sin6_addr = in6addr_any;

        if (connect(socks, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            perror("Connect failed");
            exit(1);
        }
    }

    while (1)
    {
        memset(client_message, 0, sizeof(client_message));
        read_size = recv(sock, client_message, sizeof(client_message), 0);
        if (read_size > 0)
        {
            printf("Received message: %s\n", client_message);
            pthread_mutex_lock(&lock);
            int other_sock = client_sockets[0] == sock ? socks : client_sockets[0];
            
            if (other_sock != -1)
            {
                
                send(other_sock, client_message, strlen(client_message), 0);
            }
            pthread_mutex_unlock(&lock);
        }
        else
        {
            break; // Client disconnected or error occurred
        }
    }

    pthread_mutex_lock(&lock);
    if (sock == client_sockets[0])
    {
        client_sockets[0] = -1;
    }
    else if (sock == client_sockets[1])
    {
        client_sockets[1] = -1;
    }
    pthread_mutex_unlock(&lock);

    free(socket_desc);
    close(sock);
    return 0;
}

void *start_server(void *port_arg)
{
    int port = *(int *)port_arg;
    int server_socket, *new_sock;
    struct sockaddr_in6 server, client;
    pthread_t thread_id;

    server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Could not create socket");
        exit(1);
    }

    server.sin6_family = AF_INET6;
    server.sin6_addr = in6addr_any;
    server.sin6_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    listen(server_socket, 3);
    printf("Server started on port %d\n", port);

    int c = sizeof(struct sockaddr_in6);
    // Infinite loop to continuously accept incoming connections
    while (1)
    {
        // Allocate memory for a new socket
        new_sock = malloc(1);

        // Accept a connection on the server socket
        *new_sock = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c);

        // Check if accept was successful
        if (*new_sock < 0)
        {
            // Print error message if accept failed and continue to next iteration
            perror("Accept failed");
            continue;
        }

        // Lock the mutex before accessing shared resources
        pthread_mutex_lock(&lock);

        // Determine the index based on the port
        int index = port == PORT1 ? 0 : 1;

        // Store the new socket in the array of client sockets
        client_sockets[index] = *new_sock;

        // Unlock the mutex after accessing shared resources
        pthread_mutex_unlock(&lock);

        // Print a message indicating the connection was accepted
        printf("Connection accepted on port %d\n", port);

        // Create a new thread to handle the client connection
        if (pthread_create(&thread_id, NULL, client_handler, (void *)new_sock) < 0)
        {
            // Print error message if thread creation failed and return NULL
            perror("Could not create thread");
            return NULL;
        }
    }

    close(server_socket);
    return NULL;
}

int main()
{
    pthread_t server_thread1, server_thread2;
    int port1 = PORT1;
    int port2 = PORT2;

    // Start server on PORT1
    if (pthread_create(&server_thread1, NULL, start_server, (void *)&port1) < 0)
    {
        perror("Could not create server thread for port 1");
        return 1;
    }

    // Start server on PORT2
    if (pthread_create(&server_thread2, NULL, start_server, (void *)&port2) < 0)
    {
        perror("Could not create server thread for port 2");
        return 1;
    }

    pthread_join(server_thread1, NULL);
    pthread_join(server_thread2, NULL);

    return 0;
}
