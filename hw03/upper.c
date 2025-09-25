#include <stdio.h>   // 표준 입출력 함수 사용을 위해 포함
#include <stdlib.h>  // 표준 라이브러리 함수 사용을 위해 포함 (예: exit 함수)
#include <ctype.h>   // 문자 변환 함수(toupper)를 사용하기 위해 포함

int main(int argc, char *argv[]) {
    // 명령행 인자 확인: 프로그램이 실행될 때 인자로 파일명이 제공되었는지 확인
    if (argc != 2) {
        // 인자가 잘못되었을 경우, 올바른 사용법을 출력하고 프로그램 종료
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // 파일 열기: fopen 함수로 파일을 읽기 모드("r")로 열기
    FILE *file = fopen(argv[1], "r");
    // 파일을 여는 데 실패했을 경우 에러 처리
    if (file == NULL) {
        perror("Error opening file");  // 오류 메시지를 출력하고
        return 1;                      // 프로그램을 종료
    }

    // 파일 내용 출력: 원래 파일의 내용을 출력
    printf("Original text:\n");
    char ch;  // 파일에서 읽어올 문자를 저장할 변수
    while ((ch = fgetc(file)) != EOF) {  // 파일의 끝(EOF)에 도달할 때까지 한 문자씩 읽기
        putchar(ch);  // 읽은 문자를 출력
    }

    // 파일 포인터를 처음으로 되돌림: 파일을 처음부터 다시 읽기 위해 rewind 사용
    rewind(file);

    // 대문자로 변환하여 출력
    printf("\nUppercase text:\n");
    while ((ch = fgetc(file)) != EOF) {   // 파일의 끝까지 다시 한 문자씩 읽기
        putchar(toupper(ch));             // 읽은 문자를 대문자로 변환하여 출력
    }

    // 파일 닫기: 모든 작업이 끝난 후 파일을 닫아 메모리 누수 방지
    fclose(file);

    return 0;  // 정상적으로 프로그램 종료
}

