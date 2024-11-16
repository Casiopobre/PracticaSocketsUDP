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
nombre del archivo como parámetros en línea de comandos por ese orden.
*/

int main(int argc, char **argv)
{
    // Verifica si los argumentos recibidos desde la línea de comandos son correctos
    if (argc < 4)
    {
        perror("\nNúmero incorrecto de argumentos\n");
        exit(EXIT_FAILURE);
    }

    // Declarar las variables
    int tSock, ownPort = atoi(argv[1]), remotePort = atoi(argv[3]), bytesSent;
    char *remoteIP = argv[2], receivedMessage[MESSAGE_LEN];

    struct sockaddr_in ownSocketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t socketSize = sizeof(struct sockaddr_in);

    // Crear el socket
    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tSock < 0)
    {
        perror("\nNo se pudo crear el socket\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar las estructuras
    ownSocketAddress.sin_family = AF_INET;
    ownSocketAddress.sin_port = htons(ownPort);
    ownSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    remoteSocketAddress.sin_family = AF_INET;
    remoteSocketAddress.sin_port = htons(remotePort);
    inet_pton(AF_INET, remoteIP, &remoteSocketAddress.sin_addr);

    // Asignar la dirección al socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nNo se pudo asignar la dirección al socket\n");
        exit(EXIT_FAILURE);
    }

    // Abrir los archivos
    FILE *lowercaseFile = fopen(argv[4], "r");
    if (lowercaseFile == NULL)
    {
        perror("Error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }

    // Archivo donde se guardarán los datos
    char *uFileName = malloc(strlen(argv[4]) * 4);
    for (int i = 0; i < strlen(argv[4]); i++)
        uFileName[i] = toupper(argv[4][i]);
    FILE *uppercaseFile = fopen(uFileName, "w");
    if (uppercaseFile == NULL)
    {
        perror("Error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }

    // Enviar el mensaje
    char buffer[MESSAGE_LEN];
    ssize_t totalBytesSent = 0;
    ssize_t totalBytesReceived = 0;

    // Bucle principal para leer el archivo
    while (fgets(buffer, sizeof(buffer), lowercaseFile) != NULL)
    {
        // sleep(3); // Pausa para pruebas en el ejercicio 3
        // printf("%s\n", buffer);
        bytesSent = sendto(tSock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
        if (bytesSent < 0)
        {
            perror("Error al enviar el mensaje al servidor\n");
            exit(EXIT_FAILURE);
        }
        totalBytesSent += bytesSent;
        // printf("Mensaje enviado: %s\n", buffer);
        // printf("Bytes enviados: %d\n", bytesSent);

        // Recibir mensaje del servidor y guardarlo en el archivo
        socklen_t addressLen = sizeof(remoteSocketAddress);
        ssize_t bytesReceived = recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &addressLen);
        if (bytesReceived < 0)
        {
            perror("Error al recibir el mensaje del servidor\n");
            exit(EXIT_FAILURE);
        }
        receivedMessage[MESSAGE_LEN] = '\0';
        totalBytesReceived += bytesReceived;
        fprintf(uppercaseFile, "%s", receivedMessage);
    }

    printf("~Bytes enviados: %ld\n", totalBytesSent);
    printf("~Bytes recibidos: %ld\n", totalBytesReceived);

    close(tSock);
    free(uFileName);
    fclose(lowercaseFile);
    fclose(uppercaseFile);
    return 1;
}