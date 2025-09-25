#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num1> <num2>\n", argv[0]);
        return 1;
    }

    // 명령줄 인자로 받은 문자열을 정수로 변환
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[2]);

    // 두 수의 합 계산
    int sum = num1 + num2;

    // 결과 출력
    printf("%d + %d = %d\n", num1, num2, sum);

    return 0; // 프로그램 정상 종료
}

