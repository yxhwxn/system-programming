#include <stdio.h>
#include <unistd.h>

// 전역 변수로 파이프를 정의
int pfd1[2], pfd2[2];

// 동기화 초기화 함수
void TELL_WAIT() {
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0) {
        perror("pipe");
        _exit(1);
    }
}

// 부모가 자식에게 알림
void TELL_CHILD() {
    if (write(pfd1[1], "c", 1) != 1) {
        perror("write");
        _exit(1);
    }
}

// 자식이 부모에게 알림
void TELL_PARENT() {
    if (write(pfd2[1], "p", 1) != 1) {
        perror("write");
        _exit(1);
    }
}

// 부모가 자식의 신호를 기다림
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

// 자식이 부모의 신호를 기다림
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

