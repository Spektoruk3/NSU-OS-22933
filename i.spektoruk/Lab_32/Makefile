CC = gcc
all: server client test
server: server.c
	$(CC) server.c -o server -lsocket -lnsl -laio

client: client.c
	$(CC) client.c -o client -lsocket -lnsl -laio

test: test.c
	$(CC) test.c -o test -lsocket -lnsl -laio

clean:
	rm *o server client test

