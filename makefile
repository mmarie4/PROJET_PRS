all : server clean

server : server.o functions.o
	gcc -o server server.o functions.o

functions.o : functions.c
	gcc -c functions.c -o functions.o

server.o : server.c
	gcc -c server.c -o server.o

clean : 
	rm -f *.o
