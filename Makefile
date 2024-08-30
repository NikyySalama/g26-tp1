CC = gcc
CFLAGS = -Wall -g

all: main slave

main: main.c
	$(CC) $(CFLAGS) -o main main.c

hijo: hijo.c
	$(CC) $(CFLAGS) -o slave slave.c

clean:
	rm -f main slave