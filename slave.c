// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "globals.h"
#include "pipe_lib.h"
#include "error.h"

#define CONTENIDO_BUFFER "%d"DELIMITER"%s"DELIMITER"%s\t" // PID, FILE, MD5

int main(int argc, char *argv[]) {
    char file_name[FILENAME_MAX];
    char mds5[MD5_SIZE+1];
    TPipe md5_pipe;
    int pipe1[2];

    while (fgets(file_name, FILENAME_MAX, stdin) != NULL) {
        file_name[strcspn(file_name, "\n")] = '\0';

        if (pipe(pipe1) == -1)
            ERROR_HANDLING(SLAVE_MD5_PIPE_CREATION); 
        
        md5_pipe.fdR = pipe1[R_END];
        md5_pipe.fdW = pipe1[W_END];


        pid_t pid = fork();
        if (pid == -1)
            ERROR_HANDLING(PROCESS_CREATING);

        if (pid == 0) {
            close(md5_pipe.fdR);
            dup2(md5_pipe.fdW, STDOUT_FILENO);
            close(md5_pipe.fdW);

            execlp("md5sum", "md5sum", file_name, NULL);
            
            // Por si falla execlp
            ERROR_HANDLING(CHILD_PROCESS_EXECUTING);
        } else { // Parent process
            close(md5_pipe.fdW);
            wait(NULL);

            if (read(md5_pipe.fdR, mds5, MD5_SIZE + 1) <= 0) 
                ERROR_HANDLING(PIPE_READING);

            close(md5_pipe.fdR);
            
            printf(CONTENIDO_BUFFER, getpid(), file_name, mds5);
            fflush(stdout);
        }
    }

    return 0;
}

