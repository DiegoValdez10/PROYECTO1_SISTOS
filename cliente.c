#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_USERNAME_LENGTH 32

void sendMessageToServer(int sockfd, char *message) {
    send(sockfd, message, strlen(message), 0);
}

void receiveMessageFromServer(int sockfd, char *buffer) {
    memset(buffer, 0, MAX_BUFFER_SIZE);
    recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <username> <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    char *username = argv[1];
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creando socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error conectandose al server");
        close(sockfd);
        return 1;
    }

    // Registrar usuario
    char register_message[MAX_BUFFER_SIZE];
    snprintf(register_message, MAX_BUFFER_SIZE, "Registro: %s", username);
    sendMessageToServer(sockfd, register_message);

    char buffer[MAX_BUFFER_SIZE];
    receiveMessageFromServer(sockfd, buffer);
    if (strcmp(buffer, "Registrado!") != 0) {
        printf("Error registrando: %s\n", buffer);
        close(sockfd);
        return 1;
    }

    printf("Registro Exitoso!!\n");

    while (1) {
        printf("Ingrasa una opcion: ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "broadcast") == 0) {
            printf("Introduce un mensaje: ");
            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            char broadcast_message[MAX_BUFFER_SIZE];
            snprintf(broadcast_message, MAX_BUFFER_SIZE, "BROADCAST %s", buffer);
            sendMessageToServer(sockfd, broadcast_message);
        } else if (strcmp(buffer, "directo") == 0) {
            printf("Ingrese el nombre de usuario del destinatario: ");
            char recipient[MAX_USERNAME_LENGTH];
            fgets(recipient, MAX_USERNAME_LENGTH, stdin);
            recipient[strcspn(recipient, "\n")] = 0;

            printf("Introduce un mensaje: ");
            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            char direct_message[MAX_BUFFER_SIZE];
            snprintf(direct_message, MAX_BUFFER_SIZE, "DIRECT %s %s", recipient, buffer);
            sendMessageToServer(sockfd, direct_message);
        } else if (strcmp(buffer, "status") == 0) {
            printf("Ingrese el nuevo estado (activo, ocupado, inactivo): ");
            fgets(buffer, MAX_BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            char status_message[MAX_BUFFER_SIZE];
            snprintf(status_message, MAX_BUFFER_SIZE, "STATUS %s", buffer);
            sendMessageToServer(sockfd, status_message);
        } else if (strcmp(buffer, "lista") == 0) {
            sendMessageToServer(sockfd, "LISTA");
            receiveMessageFromServer(sockfd, buffer);
            printf("%s\n", buffer);
        } else if (strcmp(buffer, "info") == 0) {
            printf("Introduce usuario: ");
            char target_username[MAX_USERNAME_LENGTH];
            fgets(target_username, MAX_USERNAME_LENGTH, stdin);
            target_username[strcspn(target_username, "\n")] = 0;

            char info_message[MAX_BUFFER_SIZE];
            snprintf(info_message, MAX_BUFFER_SIZE, "INFO %s", target_username);
            sendMessageToServer(sockfd, info_message);
            receiveMessageFromServer(sockfd, buffer);
            printf("%s\n", buffer);
        } else if (strcmp(buffer, "help") == 0) {
             printf("Comandos disponibles:\n");
             printf("transmitir - Enviar un mensaje a todos los usuarios conectados\n");
             printf("direct - Enviar un mensaje directo a un usuario específico\n");
             printf("status - Cambia tu estado (activo, ocupado, inactivo)\n");
             printf("lista - Listar todos los usuarios conectados\n");
             printf("info - Obtener información sobre un usuario específico\n");
             printf("salir - Salir de la aplicación de chat\n");
        } else if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
            printf("Comando invalido. Escribe 'help' para mas comandos\n");
        }
    }

    close(sockfd);
    return 0;
}