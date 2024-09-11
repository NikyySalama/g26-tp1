// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "globals.h"

#define CONTENIDO_BUFFER "%d"DELIMITER"%s"DELIMITER"%s""\n" // PID, FILE, MD5

int main(int argc, char *argv[]) {
    //srand(time(NULL)^ getpid());
    //int random_time = (rand() % 5) + 1;
    char buffer[FILENAME_MAX];
    char mds5[MD5_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
    
    while (fgets(buffer, FILENAME_MAX, stdin) != NULL) {
        // procesa el file e imprime su md5:
        //usleep(random_time * 80000);
        buffer[FILENAME_MAX] = '\0';
        printf(CONTENIDO_BUFFER, getpid(), buffer, mds5);
        fflush(stdout);
    }

    return 0;
}