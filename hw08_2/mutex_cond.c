/*
 * mutex_cond.c
 *
 * 이 구현은 요청/응답 패턴을 하나의 프로세스 내 두 스레드로 구현하며,
 * Mutex와 Condition Variable을 사용한 동기화를 보여준다.
 *
 * [동기화 방식의 특징]
 * - Mutex + Condition Variable:
 *   - 서로 다른 스레드가 공유 데이터에 접근할 때 상호배제를 위한 Mutex 사용.
 *   - 특정 상태(flag==1 등)에서만 진행하도록 Condition Variable로 대기/신호를 구현.
 *   - 조건 기반 대기(조건이 충족될 때까지 cond_wait) 및 명시적 신호(cond_signal) 가능.
 *   - 복잡한 조건 처리에 유용하며, 상태 기반 흐름 제어가 직관적.
 *
 * [Semaphore 방식과 비교]
 * - Condition Variable은 상태 변화를 명시적으로 표현하기 쉽다.
 * - 여러 조건이나 복잡한 상태 변환이 있을 때 Cond Variable이 더 직관적일 수 있다.
 * - 반면, Semaphore 방식은 단순히 신호(이벤트) 발생/대기를 처리하는데 적합하며, 코드가 조금 더 단순할 수 있다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// 전역 변수
char message[1024];
int flag = 0; // 0: 요청 대기 상태, 1: 요청 전달 완료 상태

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *server_thread(void *arg) {
    pthread_mutex_lock(&mutex);
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
    }

    // 요청 수신
    printf("Received request (Mutex/Cond): %s\n", message);

    // 응답 생성
    snprintf(message, sizeof(message), "This is a reply from server thread (Mutex/Cond).");
    printf("Replied (Mutex/Cond).\n");

    flag = 0;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *client_thread(void *arg) {
    pthread_mutex_lock(&mutex);

    // 요청 작성
    snprintf(message, sizeof(message), "This is a request from client thread (Mutex/Cond).");
    printf("Sent a request (Mutex/Cond).....\n");

    flag = 1;
    pthread_cond_signal(&cond);

    while (flag == 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    // 응답 수신
    printf("Received reply (Mutex/Cond): %s\n", message);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t tid_server, tid_client;

    // 어떤 방식을 사용하는지 출력
    printf("** Using Mutex + Condition Variable **\n");
    printf("** Condition Variable allows easy state-based waiting/signaling **\n");

    if (pthread_create(&tid_server, NULL, server_thread, NULL) != 0) {
        perror("pthread_create server");
        exit(1);
    }

    sleep(1);

    if (pthread_create(&tid_client, NULL, client_thread, NULL) != 0) {
        perror("pthread_create client");
        exit(1);
    }

    pthread_join(tid_server, NULL);
    pthread_join(tid_client, NULL);

    return 0;
}

