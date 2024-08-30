CC = gcc
CFLAGS = -Wall -g

all: main view

main: main.c globals.o
	$(CC) $(CFLAGS) -o main main.c globals.o

view: view.c globals.o
	$(CC) $(CFLAGS) -o view view.c globals.o

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

clean:
	rm -f main view globals.o