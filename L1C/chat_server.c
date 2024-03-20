#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

// Constants
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
#define NAME_SIZE 50
#define MAX_MESSAGE_SIZE 1024

// Global 
int clientSockets[MAX_CLIENTS];
char clientNames[MAX_CLIENTS][NAME_SIZE];
int numClients = 0;
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototype
void *clientHandler(void *arg);

int main(int argc, char *argv[])
{
    int serverFd, newSocket;
    struct sockaddr_in6 address;
    int opt = 1;
    int no = 0;
    int addrlen = sizeof(address);
    pthread_t threadId;

    // Check if port is provided
    if ((serverFd = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(serverFd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) < 0)
    {
        perror("setsockopt IPV6_V6ONLY failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address)); 
    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = htons(atoi(argv[1]));

    // Bind socket to port
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverFd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Serveris veikia...\n\n");

    // Accept incoming connections
    while (1)
    {
        // Accept a connection on the server socket
        if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Create a new thread for the client
        if (pthread_create(&threadId, NULL, clientHandler, (void *)&newSocket) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void *clientHandler(void *arg)
{
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char message[MAX_MESSAGE_SIZE];
    char name[NAME_SIZE] = {0};

    // Request, receive and trim the client's name
    send(sock, "ATSIUSKVARDA\n", strlen("ATSIUSKVARDA\n"), 0);

    ssize_t nameLen = recv(sock, name, NAME_SIZE, 0);
    if (nameLen <= 0)
    {
        close(sock);
        pthread_exit(NULL);
    }

    int nameL = strlen(name);
    for (int i = 0; i < nameL; i++)
    {
        if (name[i] == '\n' || name[i] == '\r')
        {
            name[i] = '\0';
        }
    }

    // Respond that the name was received
    send(sock, "VARDASOK\n", strlen("VARDASOK\n"), 0);

    // Adds a client to a list of clients
    pthread_mutex_lock(&clientsMutex);
    strncpy(clientNames[numClients], name, NAME_SIZE);
    clientSockets[numClients++] = sock;
    pthread_mutex_unlock(&clientsMutex);

    while (1)
    {
        // Receives and trims the message from the client
        ssize_t bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0)
        {
            break;
        }
        
        int bufferLen = strlen(buffer);
        for (int i = 0; i < bufferLen; i++)
        {
            if (buffer[i] == '\n' || buffer[i] == '\r')
            {
                buffer[i] = '\0';
            }
        }

        printf("Received message: %s\n", buffer);
        printf("Received from: %s\n\n", name);

        snprintf(message, sizeof(message), "PRANESIMAS %s: %s\n", name, buffer);
        
        pthread_mutex_lock(&clientsMutex);
        for (int i = 0; i < numClients; i++)
        {
            send(clientSockets[i], message, strlen(message), 0);
        }
        pthread_mutex_unlock(&clientsMutex);
    }

    // Remove the client from the list
    pthread_mutex_lock(&clientsMutex);
    for (int i = 0; i < numClients; i++)
    {
        // Find the client socket to remove
        if (clientSockets[i] == sock)
        {
            // Shift the remaining clients to fill the gap
            for (int j = i; j < numClients - 1; j++)
            {
                clientSockets[j] = clientSockets[j + 1];
                strncpy(clientNames[j], clientNames[j + 1], NAME_SIZE);
            }
            numClients--;
            break; 
        }
    }

    // Unlock the mutex and close the socket
    pthread_mutex_unlock(&clientsMutex);
    close(sock);
    pthread_exit(NULL);
}