#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 각 쓰레드의 결과를 저장할 전역 변수
int sum1 = 0;
int sum2 = 0;

// 1~50까지 합을 구하는 함수
void *sumUpTo50(void *arg) {
    for (int i = 1; i <= 50; i++) {
        sum1 += i;
    }
    pthread_exit(NULL); // 쓰레드 종료
}

// 51~100까지 합을 구하는 함수
void *sumFrom51To100(void *arg) {
    for (int i = 51; i <= 100; i++) {
        sum2 += i;
    }
    pthread_exit(NULL); // 쓰레드 종료
}

int main() {
    pthread_t thread1, thread2;

    // 첫 번째 쓰레드 생성 (1~50 합 계산)
    if (pthread_create(&thread1, NULL, sumUpTo50, NULL) != 0) {
        perror("Failed to create thread1");
        return 1;
    }

    // 두 번째 쓰레드 생성 (51~100 합 계산)
    if (pthread_create(&thread2, NULL, sumFrom51To100, NULL) != 0) {
        perror("Failed to create thread2");
        return 1;
    }

    // 각 쓰레드가 종료되기를 기다림
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 최종 결과 계산 및 출력
    int totalSum = sum1 + sum2;
    printf("Total sum from 1 to 100: %d\n", totalSum);

    return 0;
}

