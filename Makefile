CFLAGS = -Wall -g -O0
LDFLAGS = -lm

LIBS = shared_memory_lib.o semaphore_lib.o utils.o

all: main slave view

main: main.c $(LIBS) error.h
	$(CC) $(CFLAGS) -o main main.c $(LIBS) $(LDFLAGS)

slave: slave.c $(LIBS) error.h
	$(CC) $(CFLAGS) -o slave slave.c $(LIBS) $(LDFLAGS)

view: view.c $(LIBS) error.h
	$(CC) $(CFLAGS) -o view view.c $(LIBS) $(LDFLAGS)

utils: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

shared_memory_lib: shared_memory_lib.c shared_memory_lib.h globals.h error.h
	$(CC) $(CFLAGS) -c shared_memory_lib.c

semaphore_lib: semaphore_lib.c semaphore_lib.h globals.h error.h
	$(CC) $(CFLAGS) -c semaphore_lib.c

clean:
	rm -f main slave view utils semaphore_lib
	rm -f main slave view $(LIBS) PVS-Studio.log PVS-Studio.err strace_out

test: pvs valgrind

VALGRIND_ARGS ?= ./main ./md5Files/*

pvs: clean
	@echo "Corriendo PVS-Studio analisis..."
	pvs-studio-analyzer trace -- make all
	pvs-studio-analyzer analyze -o PVS-Studio.log
	plog-converter -a GA:1,2,3 -t errorfile PVS-Studio.log -o PVS-Studio.err
	@echo "Analisis de PVS-Studio completado. Mirar PVS-Studio.err para los resultados."

valgrind:
	@echo "Corriendo Valgrind analisis..."
	@/bin/sh -c "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(VALGRIND_ARGS)"
	@echo "Valgrind analisis completado."

.PHONY: all clean test pvs valgrind