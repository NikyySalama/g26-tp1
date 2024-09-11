// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define CONTENIDO_BUFFER "%d"DELIMITER"%s"DELIMITER"%s\n" // PID, FILE, MD5

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <pipe_in_fd> <pipe_out_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // File descriptors for pipes
    int pipe_in_fd = atoi(argv[1]);  // Read file path from the master
    int pipe_out_fd = atoi(argv[2]); // Send MD5 result to the master

    char filename[BUFFER_SIZE];
    char md5_result[BUFFER_SIZE];

    // Read the file name from the pipe
    if (read(pipe_in_fd, filename, BUFFER_SIZE) <= 0) {
        perror("Error reading file name from pipe");
        exit(EXIT_FAILURE);
    }

    // Remove potential newline from the file name
    filename[strcspn(filename, "\n")] = '\0';

    // Create a pipe for capturing md5sum output
    int md5_pipe[2];
    if (pipe(md5_pipe) == -1) {
        perror("Error creating pipe for md5sum");
        exit(EXIT_FAILURE);
    }

    // Fork a child process to execute md5sum
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error forking for md5sum");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Redirect the child's stdout to the pipe
        close(md5_pipe[0]); // Close reading end
        dup2(md5_pipe[1], STDOUT_FILENO); // Redirect stdout to the writing end of the pipe
        close(md5_pipe[1]); // Close the writing end (dup2 already took over)

        // Execute md5sum for the given file
        execlp("md5sum", "md5sum", filename, NULL);
        
        // If execlp fails
        perror("Error executing md5sum");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(md5_pipe[1]); // Close the writing end in the parent

        // Wait for the child process to finish
        wait(NULL);

        // Read the MD5 result from the pipe
        if (read(md5_pipe[0], md5_result, MD5_SIZE + 1) <= 0) {
            perror("Error reading md5sum output");
            exit(EXIT_FAILURE);
        }

        close(md5_pipe[0]); // Close the reading end of the md5 pipe

        // Prepare the result string (MD5 + file name)
        char result[BUFFER_SIZE];
        snprintf(result, sizeof(result), "File: %s, MD5: %.*s\n", filename, MD5_SIZE, md5_result);

        // Write the result back to the master via the pipe
        if (write(pipe_out_fd, result, strlen(result)) == -1) {
            perror("Error writing to output pipe");
            exit(EXIT_FAILURE);
        }

        close(pipe_out_fd); // Close the pipe to indicate the slave is done
    }

    return 0;
}
