#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#define PETITIONS_NUM 10
#define MESSAGE_LEN 1024

// Se le pasa el numero de puerto por el que atiende las solicitudes
int main(int argc, char **argv)
{
    // Check if the args received from the command line are correct
    if (argc < 2)
    {
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Declare the variables
    int tSock, ownPort = atoi(argv[1]), remotePort = atoi(argv[2]);
    struct sockaddr_in ownSocketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t socketSize = sizeof(struct sockaddr_in);

    // Create the socket
    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tSock < 0)
    {
        perror("\nUnable to create socket\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the structs
    ownSocketAddress.sin_family = AF_INET;
    ownSocketAddress.sin_port = htons(ownPort);
    ownSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    remoteSocketAddress.sin_family = AF_INET;
    remoteSocketAddress.sin_port = htons(remotePort);

    // Assign address and port to the socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Receive initial message from a new client
        char receivedMessage[MESSAGE_LEN];
        socklen_t tamano = sizeof(struct sockaddr_in);
        ssize_t bytesReceived = recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &tamano);
        if (bytesReceived < 0)
        {
            perror("Error receiving initial message from client\n");
            continue;
        }

        // Display client's IP and port
        char iptext[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, (const void *)&remoteSocketAddress.sin_addr.s_addr, iptext, INET_ADDRSTRLEN) != NULL)
        {
            printf("Connected to client with IP %s and port %d\n", iptext, ntohs(remoteSocketAddress.sin_port));
        }
        else
        {
            perror("Error with IP address:");
        }

        // Loop to handle messages from the current client
        while (1)
        {
            // Receive a message from the client
            bytesReceived = recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &tamano);
            if (bytesReceived < 0)
            {
                perror("Error receiving message from client\n");
                break;
            }
            else if (bytesReceived == 0)
            {
                printf("Client disconnected\n");
                break;
            }
            receivedMessage[bytesReceived] = '\0';
            printf("Mensaje recibido: %s\n", receivedMessage);

            // Transform the message to uppercase
            for (int i = 0; i < bytesReceived; i++)
                receivedMessage[i] = toupper(receivedMessage[i]);

            // Send the uppercase message back to the client
            ssize_t bytesSent = sendto(tSock, receivedMessage, bytesReceived, 0, (struct sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
            if (bytesSent < 0)
            {
                perror("Error sending message to client\n");
                break;
            }
        }

        // This client has disconnected; wait for a new client
        printf("Waiting for a new client...\n");
    }

    // Close the socket
    close(tSock);

    return 0;
}
