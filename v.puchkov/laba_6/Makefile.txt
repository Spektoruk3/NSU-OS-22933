main: main.o
	gcc main.o -o main

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm *.o main
