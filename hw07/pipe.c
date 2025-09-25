#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_BUF 128

int main() {
    int fd1[2], fd2[2];
    pid_t pid;
    char buf[MAX_BUF];

    // 파이프 생성
    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("pipe");
        exit(1);
    }

    // fork 생성
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) { // Child process
        close(fd1[1]); // Parent -> Child 쓰기 닫기
        close(fd2[0]); // Child -> Parent 읽기 닫기

        printf("Child: Wait for parent to send data\n");
        if (read(fd1[0], buf, MAX_BUF) < 0) {
            perror("read");
            exit(1);
        }
        printf("Child: Received data from parent: %s", buf);

        // Child가 Parent에게 응답
        strcpy(buf, "Hello, Parent!\n");
        printf("Child: Send data to parent\n");
        write(fd2[1], buf, strlen(buf) + 1);

        close(fd1[0]);
        close(fd2[1]);
    }
    else { // Parent process
        close(fd1[0]); // Parent -> Child 읽기 닫기
        close(fd2[1]); // Child -> Parent 쓰기 닫기

        // Parent가 Child에게 메시지 전송
        strcpy(buf, "Hello, Child!\n");
        printf("Parent: Send data to child\n");
        write(fd1[1], buf, strlen(buf) + 1);

        // Parent가 Child로부터 메시지 수신
        if (read(fd2[0], buf, MAX_BUF) < 0) {
            perror("read");
            exit(1);
        }
        printf("Parent: Received data from child: %s", buf);

        close(fd1[1]);
        close(fd2[0]);
    }

    return 0;
}

