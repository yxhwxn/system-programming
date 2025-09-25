/*
과제 설명

- Parent process가 child process의 종료시, SIGCHLD signal을 asynchronous하게 처리하는 프로그램을 작성
- 즉, child process의 종료를 synchronous하게 처리하기 위해서는 wait() system call을 사용하고, asynchronous하게 처리하기 위해서는 signal을 사용함을 익힘
- [Hint] Processes and Threads의 wait.c 실습 내용에서 wait 부분을 while (1) ; 로 변경하고, fork() 이전에 signal handler를 등록
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

// Signal handler for SIGCHLD
void handle_sigchld(int sig) {
    int status; // 자식 프로세스의 종료 상태 저장
    pid_t pid;  // 종료된 자식 프로세스의 PID 저장

    // 종료된 모든 자식 프로세스를 수거
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process with PID %d terminated with status %d\n", pid, status);
    }
}

int main() {
    pid_t pid;

    // SIGCHLD 신호를 처리할 핸들러 등록
    signal(SIGCHLD, handle_sigchld);

    // fork() 호출로 자식 프로세스 생성
    if ((pid = fork()) < 0) {
        // fork 실패 시 에러 처리
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // 자식 프로세스 실행 블록
        printf("I'm a child process with PID %d\n", getpid());
        sleep(2); // 2초 대기 후 종료
        exit(42); // 종료 상태 42 반환
    } else {
        // 부모 프로세스 실행 블록
        printf("Parent process waiting asynchronously\n");

        // 부모 프로세스가 계속 실행되며 다른 작업 수행
        while (1) {
            printf("Parent process is doing some work...\n");
            sleep(1); // 1초 대기 후 다시 루프
        }
    }

    return 0;
}

