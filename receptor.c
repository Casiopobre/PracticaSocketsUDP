#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX_LEN 1024

/*
Debe escuchar por todas sus interfaces usando un número de puerto indicado como parámetro en línea de comandos
Debe imprimir en pantalla la IP y el puerto del que le envía el mensaje, además del mensaje recibido
*/

int main(int argc, char **argv)
{
    // Check if the args received from the command line are correct
    if (argc < 2)
    {
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    char receivedMessage[MAX_LEN];

    // Declare the variables
    int tSock, ownPort = atoi(argv[1]), remotePort = atoi(argv[3]);
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
    // inet_pton(AF_INET, remoteIP, &remoteSocketAddress.sin_addr);

    // Assign addreess to the socket
    if (bind(tSock, (struct sockaddr *) &ownSocketAddress, socketSize) != 0)
    {
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }

    int bytesIn = recvfrom(tSock, receivedMessage, sizeof(receivedMessage), 0, (struct sockaddr *) &remoteSocketAddress, &socketSize);

    printf("Bytes received: %d\n", bytesIn);
    printf("%f\n", receivedMessage);
    char iptext[INET_ADDRSTRLEN];
    if(inet_ntop(AF_INET, (const void *)&remoteSocketAddress.sin_addr.s_addr, iptext, INET_ADDRSTRLEN)!=NULL){
            printf("~Server ip: %s\n~Port: %d\n", iptext, ntohs(remoteSocketAddress.sin_port));
    }else{
        perror("Ha habido un error con la ip:");
    }

    // Close the socket
    close(tSock);
}
