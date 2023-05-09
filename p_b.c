#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 256

int main(void) {
    pid_t pid[3];
    int fd[6][2]; // 3 pipes, each with a read and a write end
    char buf[BUF_SIZE];
    int n;

    // Create three pipes
    for (int i = 0; i < 3; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // Create three child processes
    for (int i = 0; i < 3; i++) {
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
            exit(1);
        } else if (pid[i] == 0) { // Child process
            close(fd[i][0]); // Close read end of pipe
            printf("Child %d ready to read from serial line\n", i);
            while (1) {
                n = read(STDIN_FILENO, buf, BUF_SIZE);
                if (n == -1) {
                    perror("read");
                    exit(1);
                }
                if (n == 0) {
                    continue;
                }
                if (buf[0] == '\n' && buf[1] == '\n') {
                    printf("Child %d terminating\n", i);
                    close(fd[i][1]); // Close write end of pipe
                    exit(0);
                }
                write(fd[i][1], buf, n);
            }
        } else { // Parent process
            close(fd[i][1]); // Close write end of pipe
        }
    }

    // Parent process reads from pipes
    while (1) {
        fd_set set;
        FD_ZERO(&set);
        for (int i = 0; i < 3; i++) {
            FD_SET(fd[i][0], &set);
        }
        int max_fd = fd[2][0] + 1;
        select(max_fd, &set, NULL, NULL, NULL);
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(fd[i][0], &set)) {
                n = read(fd[i][0], buf, BUF_SIZE);
                if (n == -1) {
                    perror("read");
                    exit(1);
                }
                if (n == 0) {
                    continue;
                }
                write(STDOUT_FILENO, buf, n);
            }
        }
        // Check if all child processes have terminated
        int status;
        for (int i = 0; i < 3; i++) {
            if (waitpid(pid[i], &status, WNOHANG) == pid[i] && WIFEXITED(status)) {
                printf("Child %d exited with status %d\n", i, WEXITSTATUS(status));
                close(fd[i][0]); // Close read end of pipe
            }
        }
        if (pid[0] == -1 && pid[1] == -1 && pid[2] == -1) {
            printf("All child processes terminated\n");
            break;
        }
    }

    return 0;
}
