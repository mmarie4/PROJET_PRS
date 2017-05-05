all : server clean

server : server.o functions.o
	gcc -o server server.o functions.o -lpthread

functions.o : functions.c
	gcc -c functions.c -o functions.o

server.o : server.c
	gcc -c server.c -o server.o -lpthread

clean :
	rm -f *.o
