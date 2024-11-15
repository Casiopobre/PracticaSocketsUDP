#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#define MESSAGE_LEN 1024

/*
Debe permitir indicar el puerto propio, la IP, el puerto del destinatario del mensaje y el
nombre del archivo como parámetros en línea de comandos por ese orden
*/

int main(int argc, char **argv)
{
    // Check if the args received from the command line are correct
    if (argc < 4)
    {
        perror("\nIncorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }

    // Declare the variables
    int tSock, ownPort = atoi(argv[1]), remotePort = atoi(argv[3]), bytesSent;
    char *remoteIP = argv[2], receivedMessage[MESSAGE_LEN];

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
    inet_pton(AF_INET, remoteIP, &remoteSocketAddress.sin_addr);

    // Assign addreess to the socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nUnable to assign address to socket\n");
        exit(EXIT_FAILURE);
    }

    // Open the files
    FILE *lowercaseFile = fopen(argv[4], "r");
    if (lowercaseFile == NULL)
    {
        perror("Erro ao abrir o arquivo\n");
        exit(EXIT_FAILURE);
    }

    // File to be saved
    char *uFileName = malloc(strlen(argv[4]) * 4);
    for (int i = 0; i < strlen(argv[4]); i++)
        uFileName[i] = toupper(argv[4][i]);
    FILE *uppercaseFile = fopen(uFileName, "w");
    if (uppercaseFile == NULL)
    {
        perror("Erro ao abrir o arquivo\n");
        exit(EXIT_FAILURE);
    }

    // Send the message
    char buffer[MESSAGE_LEN];
    ssize_t totalBytesSent = 0;
    ssize_t totalBytesReceived = 0;

    // Main loop to read the file
    while (fgets(buffer, sizeof(buffer), lowercaseFile) != NULL)
    {
        //sleep(3); //Sleep made to test exercise 3
        // printf("%s\n",buffer);
        bytesSent = sendto(tSock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
        if (bytesSent < 0)
        {
            perror("Error sending message to server\n");
            exit(EXIT_FAILURE);
        }
        totalBytesSent += bytesSent;
        // printf("Message sent: %s\n", buffer);
        // printf("BytesSent: %d\n", bytesSent);

        // Receive message from server and save it into the file
        socklen_t addressLen = sizeof(remoteSocketAddress);
        ssize_t bytesReceived = recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &addressLen);
        if (bytesReceived < 0)
        {
            perror("Error receiving message from server\n");
            exit(EXIT_FAILURE);
        }
        receivedMessage[MESSAGE_LEN] = '\0';
        totalBytesReceived += bytesReceived;
        fprintf(uppercaseFile, "%s", receivedMessage);
    }

    printf("~Bytes sent: %ld\n", totalBytesSent);
    printf("~Bytes received: %ld\n", totalBytesReceived);

    close(tSock);
    free(uFileName);
    fclose(lowercaseFile);
    fclose(uppercaseFile);
    return 1;
}
