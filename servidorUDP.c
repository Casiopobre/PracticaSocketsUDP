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
    if (argc < 2){
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Declare the variables
    int tSock, ownPort = atoi(argv[1]);
    struct sockaddr_in ownSocketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t socketSize = sizeof(struct sockaddr_in);

    // Create the socket
    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tSock < 0){
        perror("\nUnable to create socket\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the structs
    ownSocketAddress.sin_family = AF_INET;
    ownSocketAddress.sin_port = htons(ownPort);
    ownSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Assign address and port to the socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }

    char receivedMessage[MESSAGE_LEN];
    socklen_t tamano = sizeof(struct sockaddr_in);
    int bytesReceived;
    ssize_t bytesSent;

    printf("~Servidor levantado en el puerto %i\n",ownPort);

    while (1){
        
        // Receive initial message from a new client
        while((recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &tamano) > 0)){
            //receivedMessage[MESSAGE_LEN] = '\0';
            bytesReceived = strlen(receivedMessage)+1;
            //printf("Mensaje recibido: %s\n", receivedMessage);
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &remoteSocketAddress.sin_addr, ip, sizeof(ip));
            printf("Conexion de %s en puerto %i\n",ip,ntohs(remoteSocketAddress.sin_port));

            // Transform the message to uppercase
            for (int i = 0; i < bytesReceived; i++)
                receivedMessage[i] = toupper(receivedMessage[i]);

            // Send the uppercase message back to the client
            bytesSent = sendto(tSock, receivedMessage, bytesReceived, 0, (struct sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
            if (bytesSent < 0){
                perror("Error sending message to client\n");
                break;
            }
        }
        //printf("Total bytes sent\n");
        
    }

    // Close the socket
    close(tSock);

    return 0;
}
