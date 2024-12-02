#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {  
        printf("Child process is running (PID: %d)\n", getpid());
        // exit(0);
    } else { 
        printf("Parent process is running (PID: %d)\n", getpid());
        printf("Child process ID: %d\n", pid);
        
        sleep(10);

        wait(NULL);
        
        printf("zombie disappeared\n");

        sleep(10);
    }

    return 0;
}