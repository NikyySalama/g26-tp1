CC = gcc
CFLAGS = -Wall -g

all: main slave view shared_memory_lib semaphore_lib 

main: main.c globals.o
	$(CC) $(CFLAGS) -o main.o main.c shared_memory_lib.o semaphore_lib.o

slave: slave.c globals.o
	$(CC) $(CFLAGS) -o slave.o slave.c shared_memory_lib.o semaphore_lib.o

view: view.c globals.o
	$(CC) $(CFLAGS) -o view.o view.c shared_memory_lib.o semaphore_lib.o

shared_memory_lib: shared_memory_lib.c shared_memory_lib.h
	$(CC) $(CFLAGS) -c shared_memory_lib.c

semaphore_lib: semaphore_lib.c semaphore_lib.h
	$(CC) $(CFLAGS) -c semaphore_lib.c

clean:
	rm -f main slave