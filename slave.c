#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "globals.h"

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    //srand(time(NULL)^ getpid());
    //int random_time = (rand() % 5) + 1;
    char buffer[BUFFER_SIZE];
    char mds5[MD5_SIZE+1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
    
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // procesa el file e imprime su md5:
        //sleep(random_time);
        printf("%s\n", mds5);
        fflush(stdout);
    }

    return 0;
}