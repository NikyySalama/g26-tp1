CC = gcc
CFLAGS = -Wall -g

all: main slave

main: main.c
	$(CC) $(CFLAGS) -o main.o main.c

slave: slave.c
	$(CC) $(CFLAGS) -o slave.o slave.c

clean:
	rm -f main slave