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

int comClient(int connectionSock,struct sockaddr_in remoteSocketAddress);

int main(int argc, char **argv)
{
    // Check if the args received from the command line are correct
    if (argc < 2)
    {
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Declare the variables
    int tSock;
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

    // Assing address and port to the socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }

    char serverIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &remoteSocketAddress.sin_addr, serverIP, INET_ADDRSTRLEN);
    // printf("\n~~ Server IP: %s", serverIP);
    // printf("\n~~ Server Port: %d\n", portNum);

    while (1)
    {
        // Accept connection
        char iptext[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET, (const void *)&remoteSocketAddress.sin_addr.s_addr, iptext, INET_ADDRSTRLEN)!=NULL){
            printf("Ha enviado los datos el servidor con ip %s y puerto %d\n", iptext, ntohs(remoteSocketAddress.sin_port));
    }else{
        perror("Ha habido un error con la ip:");
    }

        comClient(tSock,remoteSocketAddress);
    }

    return 1;
}

int comClient(int tSock,struct sockaddr_in remoteSocketAddress)
{
    // Receive message from client
    while (1)
    {
        char mensajeRecibido[MESSAGE_LEN];
        socklen_t tamano = sizeof(struct sockaddr_in);
        ssize_t totalBytesReceived = 0;
        ssize_t bytesReceived = recvfrom(tSock, mensajeRecibido, 1000, 0, (struct sockaddr *)&remoteSocketAddress, &tamano);
        if (bytesReceived < 0)
        {
            perror("Error receiving message from client\n");
            exit(EXIT_FAILURE);
        }
        else if (bytesReceived == 0)
        {
            perror("Client Disconnected\n");
            return (1);
        }
        mensajeRecibido[bytesReceived] = '\0';
        // printf("ReceivedBytes: %ld\n", bytesReceived);
        // printf("Received Message: %s\n", receivedMessage);

        totalBytesReceived += bytesReceived;
        // Transform to uppercase
        for (int i = 0; i < bytesReceived; i++)
            mensajeRecibido[i] = toupper(mensajeRecibido[i]);

        // Send the message to the client
        // printf("SentMessage: %s\n", receivedMessage);
        ssize_t totalBytesSent = 0;
        ssize_t bytesSent = sendto(tSock, mensajeRecibido, bytesReceived, 0, (struct sockaddr *) &remoteSocketAddress, sizeof(remoteSocketAddress));
        if (bytesSent < 0)
        {
            perror("Error sending message to client\n");
            exit(EXIT_FAILURE);
        }
        totalBytesSent += bytesSent;
    }
    // Comprobation printfs
    // printf("Bytes sent: %ld\n", totalBytesSent);
    // printf("Bytes received: %ld\n", totalBytesReceived);

    // Close the sockets
    // close(connectionSock);}
}