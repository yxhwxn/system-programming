/*
 * semaphore_version.c
 *
 * 이 구현은 동일한 요청/응답 패턴을 세마포어를 사용해 구현한다.
 *
 * [동기화 방식의 특징]
 * - Semaphore:
 *   - 세마포어 카운트를 통해 스레드 간 신호/대기를 구현.
 *   - sem_wait로 대기, sem_post로 신호.
 *   - 단순히 "이벤트가 발생했는지" 여부를 기다리는 경우 간단하고 효율적.
 *
 * [Mutex/Cond와 비교]
 * - Semaphore는 단순히 카운트 업/다운을 통한 동기화이므로 직관적으로 조건을 표현하기 어렵다.
 * - 다양한 상태 변화를 처리하기보다는 단순한 "요청 대기, 응답 대기" 형태의 이벤트 동기화에 적합하다.
 * - 구현이 상대적으로 단순하지만, 조건 기반 상태 제어는 Mutex/Cond보다 덜 명확할 수 있다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

char message[1024];

sem_t sem_req; // 요청 신호용 세마포어
sem_t sem_rep; // 응답 신호용 세마포어

void *server_thread(void *arg) {
    sem_wait(&sem_req);

    // 요청 수신
    printf("Received request (Semaphore): %s\n", message);

    // 응답 작성
    snprintf(message, sizeof(message), "This is a reply from server thread (Semaphore).");
    printf("Replied (Semaphore).\n");

    sem_post(&sem_rep);
    return NULL;
}

void *client_thread(void *arg) {
    snprintf(message, sizeof(message), "This is a request from client thread (Semaphore).");
    printf("Sent a request (Semaphore).....\n");

    sem_post(&sem_req);

    sem_wait(&sem_rep);
    printf("Received reply (Semaphore): %s\n", message);

    return NULL;
}

int main() {
    pthread_t tid_server, tid_client;

    // 어떤 방식을 사용하는지 출력
    printf("** Using Semaphore **\n");
    printf("** Semaphore provides simple event-based signaling **\n");

    if (sem_init(&sem_req, 0, 0) < 0) {
        perror("sem_init sem_req");
        exit(1);
    }

    if (sem_init(&sem_rep, 0, 0) < 0) {
        perror("sem_init sem_rep");
        exit(1);
    }

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

    sem_destroy(&sem_req);
    sem_destroy(&sem_rep);

    return 0;
}

