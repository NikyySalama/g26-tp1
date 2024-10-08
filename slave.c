// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <libgen.h>
#include "globals.h"
#include "pipe_lib.h"
#include "error.h"

#define CONTENIDO_BUFFER "%d"DELIMITER"%s"DELIMITER"%s"SEPARATOR // PID, FILE, MD5

int main(int argc, char *argv[]) {
    char file_path[MAX_FILEPATH];
    char mds5[MD5_SIZE+1];
    TPipe md5_pipe;
    int pipe1[2];

    while (fgets(file_path, MAX_FILEPATH, stdin) != NULL) {
        file_path[strcspn(file_path, "\n")] = '\0';

        if (pipe(pipe1) == -1)
            ERROR_HANDLING(SLAVE_MD5_PIPE_CREATION); 
        
        md5_pipe.fd_R = pipe1[R_END];
        md5_pipe.fd_W = pipe1[W_END];


        pid_t pid = fork();
        if (pid == -1)
            ERROR_HANDLING(PROCESS_CREATING);

        if (pid == 0) {
            close(md5_pipe.fd_R);
            dup2(md5_pipe.fd_W, STDOUT_FILENO);
            close(md5_pipe.fd_W);

            execlp("md5sum", "md5sum", file_path, NULL);
            
            // Por si falla execlp
            ERROR_HANDLING(CHILD_PROCESS_EXECUTING);
        } else { // Parent process
            close(md5_pipe.fd_W);
            wait(NULL);

            if (read(md5_pipe.fd_R, mds5, MD5_SIZE + 1) <= 0) 
                ERROR_HANDLING(PIPE_READING);

            close(md5_pipe.fd_R);
            
            char *file_basename = basename((char *)file_path); // Extraemos el nombre del archivo
            
            printf(CONTENIDO_BUFFER, getpid(), file_basename, mds5);
            fflush(stdout);
        }
    }

    return 0;
}

