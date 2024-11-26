#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("in child process");
        char *args[] = {"./sequential_min_max", "10", "20", NULL};
        char *args2[] = {"10", "20"};
        execv(args[0], args2);
        perror("Exec failed\n");
        return 1;
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            fprintf(stderr, "Child process did not exit normally\n");
        }
    }

    return 0;
}