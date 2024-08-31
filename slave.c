
//slave
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    if (read(STDIN_FILENO, buffer, sizeof(buffer))) {
        printf("RESULT %s", buffer);
    }
    return 0;
}