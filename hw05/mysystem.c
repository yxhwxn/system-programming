#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// mysystem 함수 구현
int mysystem(char *cmd) {
    pid_t pid = fork(); // 새로운 프로세스 생성
    if (pid < 0) {
        perror("fork failed");
        return -1; // 실패 시 -1 반환
    } else if (pid == 0) {
        // 자식 프로세스: 명령어를 실행
        char *args[] = {"sh", "-c", cmd, NULL}; // 쉘 명령어를 실행하기 위한 인자 설정
        execv("/bin/sh", args); // /bin/sh를 통해 cmd 실행
        perror("execv failed"); // execv가 실패하면 오류 출력
        exit(1); // 실패 시 자식 프로세스 종료
    } else {
        // 부모 프로세스: 자식 프로세스의 종료를 기다림
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return -1;
        }
        // 자식 프로세스가 정상 종료되었는지 확인 후 종료 상태 반환
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); // 자식 프로세스의 종료 코드를 반환
        } else {
            return -1; // 비정상 종료 시 -1 반환
        }
    }
}

int main() {
    char command[100];
    printf("Enter a command to execute: ");
    fgets(command, sizeof(command), stdin); // 사용자로부터 명령어 입력 받기
    command[strcspn(command, "\n")] = 0; // 개행 문자 제거

    int result = mysystem(command); // mysystem 함수 호출
    printf("Command executed with exit code: %d\n", result);

    return 0;
}

