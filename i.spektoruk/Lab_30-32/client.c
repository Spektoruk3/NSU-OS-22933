#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#define MAX_BUFFER 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* receiveMessages(void* arg) {
    int sock = *((int*)arg);
    char buffer[MAX_BUFFER];
    int readCount;

    while ((readCount = read(sock, buffer, MAX_BUFFER)) > 0) {
        pthread_mutex_lock(&mutex);

        // Выводим полученное сообщение в верхнем регистре
        for (int i = 0; i < readCount; i++) {
            buffer[i] = toupper((unsigned char)buffer[i]);
        }
        printf("%s", buffer);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    int sock;
    struct sockaddr_un socketAddr;
    char* socketPath = "./sockett13";

    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, socketPath);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
        perror("connection error");
        close(sock);
        return -1;
    } else {
        printf("connection established\n");

        // Запускаем поток для приема сообщений от сервера
        pthread_t thread;
        if (pthread_create(&thread, NULL, receiveMessages, &sock) != 0) {
            perror("pthread_create");
            close(sock);
            return -1;
        }

        char readWriteBuf[MAX_BUFFER];
        int readCount;

        while ((readCount = read(STDIN_FILENO, readWriteBuf, MAX_BUFFER)) > 0) {
            pthread_mutex_lock(&mutex);

            // Отправляем введенное сообщение на сервер
            if (write(sock, readWriteBuf, readCount) == -1) {
                perror("writing error");
                close(sock);
                return -1;
            }

            pthread_mutex_unlock(&mutex);
        }

        // Если чтение с клавиатуры завершилось, закрываем соединение
        close(sock);
    }

    return 0;
}
