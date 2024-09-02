CC = gcc
CFLAGS = -Wall -g

all: main slave view

main: main.c globals.o
	$(CC) $(CFLAGS) -o main.o main.c globals.o

slave: slave.c globals.o
	$(CC) $(CFLAGS) -o slave.o slave.c globals.o

view: view.c globals.o
	$(CC) $(CFLAGS) -o view.o view.c globals.o

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

clean:
	rm -f main slave