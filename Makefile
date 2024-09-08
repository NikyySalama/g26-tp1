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

test: clean all
	# PVS-Studio analysis
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log > /dev/null

	# Valgrind memory check for main
	valgrind --leak-check=full -v ./main.o ./md5Files/*.txt | ./view.o 2> main_valgrind_report.txt

	# Valgrind memory check for view
	valgrind --leak-check=full -v ./view.o ./md5Files/*.txt 2> view_valgrind_report.txt

.PHONY: all clean test