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

// Se le pasa el número de puerto por el que atiende las solicitudes
int main(int argc, char **argv)
{
    // Verifica si los argumentos recibidos desde la línea de comandos son correctos
    if (argc < 2){
        perror("\nNúmero incorrecto de argumentos\n");
        exit(EXIT_FAILURE);
    }

    // Declarar las variables
    int tSock, ownPort = atoi(argv[1]);
    struct sockaddr_in ownSocketAddress;
    struct sockaddr_in remoteSocketAddress;
    socklen_t socketSize = sizeof(struct sockaddr_in);

    // Crear el socket
    tSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tSock < 0){
        perror("\nNo se pudo crear el socket\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar las estructuras
    ownSocketAddress.sin_family = AF_INET;
    ownSocketAddress.sin_port = htons(ownPort);
    ownSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Asignar dirección y puerto al socket
    if (bind(tSock, (struct sockaddr *)&ownSocketAddress, socketSize) != 0)
    {
        perror("\nNo se pudo asignar la dirección al socket\n");
        exit(EXIT_FAILURE);
    }

    char receivedMessage[MESSAGE_LEN];
    socklen_t tamano = sizeof(struct sockaddr_in);
    int bytesReceived;
    ssize_t bytesSent;

    printf("~Servidor levantado en el puerto %i\n", ownPort);

    while (1){
        // Recibir mensaje inicial de un cliente nuevo
        while((recvfrom(tSock, receivedMessage, MESSAGE_LEN, 0, (struct sockaddr *)&remoteSocketAddress, &tamano) > 0)){
            bytesReceived = strlen(receivedMessage) + 1;
            //printf("Mensaje recibido: %s\n", receivedMessage);
            
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &remoteSocketAddress.sin_addr, ip, sizeof(ip));
            printf("Conexión de %s en puerto %i\n", ip, ntohs(remoteSocketAddress.sin_port));

            // Transformar el mensaje a mayúsculas
            for (int i = 0; i < bytesReceived; i++)
                receivedMessage[i] = toupper(receivedMessage[i]);

            // Enviar el mensaje en mayúsculas de vuelta al cliente
            bytesSent = sendto(tSock, receivedMessage, bytesReceived, 0, (struct sockaddr *)&remoteSocketAddress, sizeof(remoteSocketAddress));
            if (bytesSent < 0){
                perror("Error al enviar el mensaje al cliente\n");
                break;
            }
        }
        //printf("Total de bytes enviados\n");
    }

    // Cerrar el socket
    close(tSock);

    return 0;
}
