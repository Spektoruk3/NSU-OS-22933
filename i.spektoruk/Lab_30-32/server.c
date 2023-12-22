#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER 256

int clientSockets[MAX_CLIENTS];
int clientCount = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* handleClient(void* arg) {
    int clientSocket = *((int*)arg);

    char buffer[MAX_BUFFER];
    int readCount;

    while ((readCount = read(clientSocket, buffer, MAX_BUFFER)) > 0) {
        pthread_mutex_lock(&mutex);

        // Преобразуем сообщение в верхний регистр
        for (int i = 0; i < readCount; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }

        // Отправка сообщения всем клиентам, кроме отправителя
        for (int i = 0; i < clientCount; ++i) {
            if (clientSockets[i] != clientSocket) {
                write(clientSockets[i], buffer, readCount);
            }
        }

        // Выводим сообщение в терминал сервера
        printf("Received from client: %s", buffer);

        pthread_mutex_unlock(&mutex);
    }

    // Закрытие сокета клиента
    close(clientSocket);

    // Удаление сокета из массива
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clientCount; ++i) {
        if (clientSockets[i] == clientSocket) {
            for (int j = i; j < clientCount - 1; ++j) {
                clientSockets[j] = clientSockets[j + 1];
            }
            --clientCount;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* serverInput(void* arg) {
    char buffer[MAX_BUFFER];
    int writeCount;

    while (1) {
        // Ожидание ввода текста от пользователя в терминале сервера
        fgets(buffer, MAX_BUFFER, stdin);

        pthread_mutex_lock(&mutex);

        // Отправка введенного текста всем клиентам
        for (int i = 0; i < clientCount; ++i) {
            writeCount = write(clientSockets[i], buffer, strlen(buffer));
            if (writeCount == -1) {
                perror("writing error");
            }
        }

        // Выводим сообщение в терминал сервера
        printf("Sent to clients: %s", buffer);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    int serverSocket;
    struct sockaddr_un socketAddr;

    // Установка адреса сокета
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    char* socketPath = "./sockett13";
    strcpy(socketAddr.sun_path, socketPath);

    // Создание сокета
    if ((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    // Привязка сокета к адресу
    if (bind(serverSocket, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("bind");
        close(serverSocket);
        return -1;
    }

    // Начало прослушивания подключений
    if (listen(serverSocket, 10) != 0) {
        perror("listen");
        close(serverSocket);
        return -1;
    }

    printf("Server is listening...\n");

    // Запуск потока для обработки ввода текста от сервера
    pthread_t inputThread;
    if (pthread_create(&inputThread, NULL, serverInput, NULL) != 0) {
        perror("pthread_create");
        close(serverSocket);
        return -1;
    }

    while (1) {
        // Принятие нового клиента
        int clientSocket = accept(serverSocket, NULL, NULL);

        if (clientSocket == -1) {
            perror("accept");
            close(serverSocket);
            return -1;
        }

        // Запуск потока для обработки клиента
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, &clientSocket) != 0) {
            perror("pthread_create");
            close(clientSocket);
            close(serverSocket);
            return -1;
        }

        // Добавление сокета клиента в массив
        pthread_mutex_lock(&mutex);
        clientSockets[clientCount++] = clientSocket;
        pthread_mutex_unlock(&mutex);
    }

    // Закрытие сокета сервера
    close(serverSocket);

    return 0;
}
