#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 10

// 각 쓰레드의 계산 결과를 저장할 배열
int partial_sums[NUM_THREADS];

// 쓰레드 함수: 주어진 범위의 합을 계산(arg: 1, 11, 21, 31, ..., 91)
void *calculate_partial_sum(void *arg) {
    int start = *(int *)arg;       // 시작값
    int end = start + 9;           // 끝값 (시작값 + 9)
    int sum = 0;

    for (int i = start; i <= end; i++) {
        sum += i;
    }

    int thread_index = (start - 1) / 10;
    partial_sums[thread_index] = sum;

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int start_values[NUM_THREADS];
    int total_sum = 0;

    // 10개의 쓰레드 생성 및 시작 값 설정
    for (int i = 0; i < NUM_THREADS; i++) {
        start_values[i] = i * 10 + 1; // 1, 11, 21, ..., 91
        if (pthread_create(&threads[i], NULL, calculate_partial_sum, &start_values[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // 모든 쓰레드가 종료될 때까지
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 쓰레드의 부분합으로 전체 합 계산
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += partial_sums[i];
    }

    printf("The sum from 1 to 100 is: %d\n", total_sum);

    return 0;
}

