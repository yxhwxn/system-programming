#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NLOOPS 5
#define BUFFER_SIZE 100

// 전역 변수 선언
char msg[BUFFER_SIZE];
sem_t sem_produce;
sem_t sem_consume;

// 첫 번째 thread
void* first_thread(void* arg) {
    // 세마포어 사용
    for (int i = 0; i < NLOOPS ; i++){
		printf("Parent: Tell to child\n");
		sem_wait(&sem_produce);
   		sem_post(&sem_consume);
		printf("Parent: Wait for child to tell\n");
    }
	return NULL;
}

// 두 번째 thread
void* second_thread(void* arg) {
    // 세마포어 사용
	for (int i = 0; i < NLOOPS ; i++) {
    	sem_wait(&sem_consume);
		printf("Child: Wait for parent to tell\n");
		printf("Child: Tell to parent\n");
    	sem_post(&sem_produce);
    }

	return NULL;
}

int main() {
    pthread_t writer, reader;  // 두 개의 thread 선언

    // 세마포어 초기화
    sem_init(&sem_produce, 0, 1);  // 초기 값 1
    sem_init(&sem_consume, 0, 0);  // 초기 값 0

    // thread 생성
    pthread_create(&writer, NULL, first_thread, NULL);
    pthread_create(&reader, NULL, second_thread, NULL);

    // thread 종료 대기
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    // 세마포어 해제
    sem_destroy(&sem_produce);
    sem_destroy(&sem_consume);

    return 0;
}
