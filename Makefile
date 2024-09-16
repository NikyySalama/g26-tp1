CFLAGS = -Wall -g
LDFLAGS = -lm

all: main slave view

main: main.c shared_memory_lib semaphore_lib utils
	$(CC) $(CFLAGS) -o main main.c shared_memory_lib semaphore_lib utils $(LDFLAGS)

slave: slave.c shared_memory_lib semaphore_lib utils
	$(CC) $(CFLAGS) -o slave slave.c shared_memory_lib semaphore_lib utils $(LDFLAGS)

view: view.c shared_memory_lib semaphore_lib utils
	$(CC) $(CFLAGS) -o view view.c shared_memory_lib semaphore_lib utils $(LDFLAGS)

utils: utils.c utils.h
	$(CC) $(CFLAGS) -o utils -c utils.c

shared_memory_lib: shared_memory_lib.c shared_memory_lib.h globals.h error.h
	$(CC) $(CFLAGS) -o shared_memory_lib -c shared_memory_lib.c

semaphore_lib: semaphore_lib.c semaphore_lib.h globals.h error.h
	$(CC) $(CFLAGS) -o semaphore_lib -c semaphore_lib.c

clean:
	rm -f main slave view utils semaphore_lib

test: clean all
	# PVS-Studio analysis
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log > /dev/null

	# Valgrind memory check for main
	valgrind --leak-check=full -v ./main ./md5Files/*.txt | ./view 2> main_valgrind_report.txt

	# Valgrind memory check for view
	valgrind --leak-check=full -v ./view ./md5Files/*.txt 2> view_valgrind_report.txt

.PHONY: all clean test