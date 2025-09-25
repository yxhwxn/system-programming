#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// 상수 정의
#define NLOOPS 10
#define SIZE sizeof(long)

// 동기화 관련 함수 대체
int pfd1[2], pfd2[2];
void TELL_WAIT() {
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0) {
        perror("pipe");
        _exit(1);
    }
}
void TELL_CHILD() {
    if (write(pfd1[1], "c", 1) != 1) {
        perror("write");
        _exit(1);
    }
}
void TELL_PARENT() {
    if (write(pfd2[1], "p", 1) != 1) {
        perror("write");
        _exit(1);
    }
}
void WAIT_CHILD() {
    char c;
    if (read(pfd2[0], &c, 1) != 1) {
        perror("read");
        _exit(1);
    }
    if (c != 'p') {
        fprintf(stderr, "WAIT_CHILD: incorrect data\n");
        _exit(1);
    }
}
void WAIT_PARENT() {
    char c;
    if (read(pfd1[0], &c, 1) != 1) {
        perror("read");
        _exit(1);
    }
    if (c != 'c') {
        fprintf(stderr, "WAIT_PARENT: incorrect data\n");
        _exit(1);
    }
}

// 업데이트 함수
int update(long *ptr) {
    return ((*ptr)++);
}

int main() {
    int shmid, i, counter;
    pid_t pid;
    long *shared_memory;

    // 공유 메모리 생성
    if ((shmid = shmget(IPC_PRIVATE, sizeof(long), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리 연결
    if ((shared_memory = (long *)shmat(shmid, NULL, 0)) == (long *)-1) {
        perror("shmat");
        exit(1);
    }

    *shared_memory = 0; // 공유 메모리 초기화

    TELL_WAIT();
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid > 0) { // Parent process
        for (i = 0; i < NLOOPS; i += 2) {
            if ((counter = update(shared_memory)) != i) {
                fprintf(stderr, "Counter mismatch\n");
                exit(1);
            }
            printf("Parent: counter=%d\n", counter);
            TELL_CHILD();
            WAIT_CHILD();
        }

        // 공유 메모리 해제
        if (shmdt(shared_memory) < 0) {
            perror("shmdt");
            exit(1);
        }
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
            exit(1);
        }
    }
    else { // Child process
        for (i = 1; i < NLOOPS; i += 2) {
            WAIT_PARENT();
            if ((counter = update(shared_memory)) != i) {
                fprintf(stderr, "Counter mismatch\n");
                exit(1);
            }
            printf("Child : counter=%d\n", counter);
            TELL_PARENT();
        }

        // 공유 메모리 해제
        if (shmdt(shared_memory) < 0) {
            perror("shmdt");
            exit(1);
        }
    }

    return 0;
}

