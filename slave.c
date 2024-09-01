#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 32

int main(int argc, char *argv[]) {
    //srand(time(NULL)^ getpid());
    //int random_time = (rand() % 5) + 1;
    char buffer[BUFFER_SIZE];
    //sleep(random_time);
    
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        printf("RESULT %s\n", buffer);
        fflush(stdout);
    }

    return 0;
}