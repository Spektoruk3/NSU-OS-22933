#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#define SOCKFILE_NAME "./30.socket"

void myexit(const char *msg, int sd) { //обработка ошибок и завершение программы
    perror(msg);
    unlink(SOCKFILE_NAME); //Удаляет файл сокета
    close(sd);
    exit(EXIT_FAILURE);  
}

struct sockaddr_un buildsockname() { //создает структуру sockaddr_un, которая содержит информацию о домене сокета и пути к файлу сокета
    struct sockaddr_un sockname;
    sockname.sun_family = PF_UNIX;
    strcpy(sockname.sun_path, SOCKFILE_NAME);
    return sockname;
}

int sockcreate() { //создает сокет
    errno = 0;
    int sd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(sd == -1) {
        myexit("socket isn't created", -1);
    }
    return sd;
}

void sockconnect(int sd) {  //устанавливает соединение с сервером
    struct sockaddr_un sockname = buildsockname();

    errno = 0;
    if(connect(sd, (struct sockaddr *)&sockname, sizeof( )) == -1) {
        myexit("connection failed", sd);
    }
}

void sockwritemsg(int sd, char *msg) { //отправляет сообщение по сокету
    errno = 0;
    if(write(sd, msg, strlen(msg) + 1) == -1) {
        myexit("writing failed", sd);
    }
}

void sockbind(int sd) { //привязывает сокет к адресу
    struct sockaddr_un sockname = buildsockname();

    errno = 0;
    if(bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1) {
        myexit("socket binding failed", sd);
    }
}

void socklisten(int sd) { //начинает прослушивание сокета
    errno = 0;
    if(listen(sd, 1) == -1) {
        myexit("listen failed", sd);
    }
}

int sockaccept(int sd) { //принимает входящее соединение
    errno = 0;
    int clientsd = accept(sd, NULL, NULL);
    if(clientsd == -1) {
        myexit("accept failed", sd);
    }
    return clientsd;
}

void msguppercase(int sd) { //преобразует принятое сообщение в верхний регистр
    char buff[255];
    errno = 0;
    while(read(sd, buff, 255) != 0) {
        printf("A message \"%s\" was read\n", buff);
        char *ptr;
        for(ptr = buff; *ptr; ptr++) {
            *ptr = toupper(*ptr);
        }
        printf("The message was uppercased: \"%s\"\n", buff);
    }
}

void intclose(int sig) { //удаление файла сокета
    unlink(SOCKFILE_NAME);
}

int main(int argc, char *argv[]) {

    //--------------------------Client-----------------------------
    if(argc > 1 && !strcmp(argv[1], "client")) {
        int clientsd = sockcreate(); //Создает сокет
        printf("Socket is created\n");

        sockconnect(clientsd); //Устанавливает соединение с сервером
        printf("Connected to a server\nPrint the message:\n");

        char msg[256] = "Default message";
        gets(msg);

        sockwritemsg(clientsd, msg); //Читает сообщение с клавиатуры и отправляет его на сервер
        printf("Message \"%s\" is sent\n", msg);

        close(clientsd);
        printf("client socket is closed\n");
    }

    //--------------------------Server-----------------------------

    else {
        int sockdes = sockcreate();
        printf("Socket is created\n");

        sockbind(sockdes); //Привязывает сокет к адресу
        printf("Socket is bound\n");

        signal(SIGINT, intclose); //Устанавливает обработчик сигнала SIGINT, чтобы можно было корректно завершить программу

        socklisten(sockdes); //Переводит сокет в режим прослушивания, ожидая входящих соединений
        printf("Program is listening for a socket. Waiting for connections...\n");

        int clientsd = sockaccept(sockdes); //Принимает входящее соединение от слушающего сокета и возвращает дескриптор принимающего сокета
        printf("Got a connection\n");

        msguppercase(clientsd); //Выполняет операцию преобразования регистра сообщения, переданного через принимающий сокет

        unlink(SOCKFILE_NAME); //Удаляет файл сокета
        printf("30.socket is removed\n");

        close(clientsd);
        printf("Reading socket is closed\n");

        close(sockdes);
        printf("The whole socket is closed\n");

    }

    return 0;

}
