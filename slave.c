#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    srand(time(NULL)^ getpid());
    int random_time = (rand() % 5) + 1;
    char buffer[BUFFER_SIZE];
    sleep(random_time);

    if (read(STDIN_FILENO, buffer, sizeof(buffer))) {
        printf("RESULT %s", buffer);
    }
    return 0;
}