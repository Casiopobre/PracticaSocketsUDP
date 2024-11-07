#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define M_LEN 1000

/*
Debe permitir indicar el puerto propio, la IP y el puerto del destinatario del mensaje como 
parámetros en línea de comandos por ese orden
Debe imprimir el número de bytes enviados
*/

int main(int argc, char **argv){
    // Check if the args received from the command line are correct
    if(argc < 3){
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Declare the variables 
    int tSock, ownPort = atoi(argv[1]), remotePort = atoi(argv[3]), bytesSent;
    char *remoteIP = argv[2];

    struct sockaddr_in ownSocketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t socketSize = sizeof(struct sockaddr_in);
    char message[M_LEN] = "Ola! It's me, Mario!\n";

    // Create the socket
    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    if(tSock < 0){
        perror("\nUnable to create socket\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the structs
    ownSocketAddress.sin_family = AF_INET;
    ownSocketAddress.sin_port = htons(ownPort);
    ownSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    remoteSocketAddress.sin_family = AF_INET;
    remoteSocketAddress.sin_port = htons(remotePort);
    inet_pton(AF_INET, remoteIP, &remoteSocketAddress.sin_addr);

    //Assign addreess to the socket
    if (bind(tSock, (struct sockaddr *) &ownSocketAddress, socketSize) != 0){
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }
    
    // Sent the message
    bytesSent = sendto(tSock, message, strlen(message)+1, 0, (struct sockaddr *) &remoteSocketAddress, sizeof(remoteSocketAddress));
    if(bytesSent == -1){
        perror("\nUnable to send message\n");
        exit(EXIT_FAILURE);
    }
    printf("\n~~ Bytes sent: %d\n", bytesSent);

    // Close the socket
    close(tSock);

}
