#include <stdio.h>      // printf() 함수 사용을 위해
#include <stdlib.h>     // exit() 함수 사용을 위해
#include <unistd.h>     // fork(), sleep() 함수 사용을 위해
#include <signal.h>     // signal(), kill() 함수 사용을 위해
#include <sys/wait.h>   // wait() 함수 사용을 위해

// 전역 변수: count
volatile sig_atomic_t count = 0;

// SIGUSR1 신호를 처리하는 핸들러 함수
void handle_sigint(int sig) {
    // 프로그램 정상 종료
    exit(0);
}

int main() {
    pid_t pid; // 프로세스 ID를 저장하는 변수

    pid = fork();

    if (pid < 0) {
        // fork 실패 시 에러 메시지 출력 후 종료
        perror("fork error");
        exit(1);
    }

    if (pid == 0) {
        // **Child Process**
        // SIGUSR1 신호를 받을 경우 handle_sigint 함수가 실행되도록 설정
        signal(SIGUSR1, handle_sigint);
    } else {
        // **Parent Process**
        sleep(1);

        // kill()을 사용해 child process에 SIGUSR1 신호를 보냄
		kill(pid, SIGUSR1);
        
		// wait()을 호출하여 child process의 종료를 기다림
        wait(NULL);
    }

    return 0;
}
