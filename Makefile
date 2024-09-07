CC = gcc
CFLAGS = -Wall -g

all: main slave view

main: main.c shared_memory_lib.o semaphore_lib.o  error.h
	$(CC) $(CFLAGS) -o main.o main.c shared_memory_lib.o semaphore_lib.o

slave: slave.c shared_memory_lib.o semaphore_lib.o globals.h  error.h
	$(CC) $(CFLAGS) -o slave.o slave.c shared_memory_lib.o semaphore_lib.o

view: view.c shared_memory_lib.o semaphore_lib.o  error.h
	$(CC) $(CFLAGS) -o view.o view.c shared_memory_lib.o semaphore_lib.o

shared_memory_lib.o: shared_memory_lib.c shared_memory_lib.h globals.h  error.h
	$(CC) $(CFLAGS) -c shared_memory_lib.c

semaphore_lib.o: semaphore_lib.c semaphore_lib.h globals.h  error.h
	$(CC) $(CFLAGS) -c semaphore_lib.c

clean:
	rm -f main slave view *.o