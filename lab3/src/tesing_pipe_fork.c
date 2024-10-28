#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 25

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork the process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {  // Child process
        close(pipefd[1]);  // Close unused write end

        // Read from the pipe
        read(pipefd[0], buffer, BUFFER_SIZE);
        printf("Child received: %s\n", buffer);

        close(pipefd[0]);
    } else {  // Parent process
        close(pipefd[0]);  // Close unused read end

        // Write to the pipe
        strcpy(buffer, "Hello from parent!");
        write(pipefd[1], buffer, strlen(buffer) + 1);

        close(pipefd[1]);
    }

    return 0;
}