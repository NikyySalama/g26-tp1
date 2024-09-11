#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "error.h"
#include "globals.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        ERROR_HANDLING("Usage: slave <read_pipe_fd> <write_pipe_fd>");
    }

    // File descriptors for communication pipes
    int pipe_in_fd = atoi(argv[1]);  // Pipe to read from (app -> slave)
    int pipe_out_fd = atoi(argv[2]); // Pipe to write to (slave -> app)

    char filePath[MAX_FILENAME];

    while (1) {
        // Reading file path from the application via stdin
        printf("Try to read on pipe_in_fd : %d\n", pipe_in_fd);

        ssize_t bytes_read = read(pipe_in_fd, filePath, sizeof(filePath) - 1);
        if (bytes_read == -1) {

            ERROR_HANDLING(PIPE_READING);

        } else if (bytes_read == 0) {
            // No more files to read, break the loop
            break;
        }

        // Null-terminate the filePath string
        filePath[bytes_read - 1] = '\0';  // Remove newline character

        // Creating a pipe for capturing the output of md5sum
        int md5_pipe[2];
        if (pipe(md5_pipe) == -1) {
            ERROR_HANDLING("Failed to create pipe for md5sum");
        }

        pid_t pid = fork();
        if (pid == -1) {
            ERROR_HANDLING(PROCESS_CREATING);
        } else if (pid == 0) {
            // Child process: Execute md5sum
            close(md5_pipe[0]);  // Close read end of pipe
            dup2(md5_pipe[1], STDOUT_FILENO);  // Redirect stdout to pipe
            execlp("md5sum", "md5sum", filePath, NULL);
            ERROR_HANDLING(CHILD_PROCESS_EXECUTING);  // If execlp fails
        } else {
            // Parent process: Read md5sum output
            close(md5_pipe[1]);  // Close write end of pipe

            char md5Output[BUFFER_SIZE];
            ssize_t md5_bytes_read = read(md5_pipe[0], md5Output, sizeof(md5Output) - 1);
            if (md5_bytes_read == -1) {
                ERROR_HANDLING(PIPE_READING);
            }
            md5Output[md5_bytes_read] = '\0';  // Null-terminate MD5 output

            // Send the result (slave ID, MD5 hash, and file name) to the application
            char resultBuffer[BUFFER_SIZE + MAX_FILENAME];
            snprintf(resultBuffer, sizeof(resultBuffer), "Slave ID: %d, MD5: %s, FILE: %s\n", getpid(), md5Output, filePath);


            if (write(pipe_out_fd, resultBuffer, strlen(resultBuffer)) == -1) {
                ERROR_HANDLING(PIPE_WRITING);
            }

            // Wait for the child process to finish
            wait(NULL);
        }
    }

    // Close the pipes before terminating
    close(pipe_in_fd);
    close(pipe_out_fd);

    return 0;
}
