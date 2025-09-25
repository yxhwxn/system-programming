#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "semlib.h"

/*
 * sipc2.c
 * - 클라이언트 역할을 하는 프로세스
 * - 공유 메모리에 요청 메세지 작성
 * - REQ_SEM에 V 연산하여 sipc1에 요청이 준비되었음을 알림
 * - REP_SEM에 P 연산으로 응답 대기
 * - 응답 수신 후 출력
 *
 * 변경 사항:
 * 1. busy waiting 제거
 * 2. System V semaphore 사용
 * 3. flag용 4byte 제거 후 전체 공유 메모리를 데이터 용도로 활용
 */

int main()
{
    int shmid, semid;
    char *ptr;
    char buf[SHM_SIZE];

    // 공유 메모리 접근
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리 붙이기
    if ((ptr = shmat(shmid, 0, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // 이미 sipc1에서 생성한 세마포어에 접근한다고 가정
    if ((semid = semGet(SEM_KEY, 2)) < 0) {
        perror("semGet");
        exit(1);
    }

    // 요청 메시지 작성
    snprintf(ptr, SHM_SIZE, "This is a request from %d.", getpid());
    printf("Sent a request.....\n");

    // 요청 신호 보내기 (REQ_SEM V)
    if (semV(semid, REQ_SEM) < 0) {
        perror("semV REQ_SEM");
        exit(1);
    }

    // 응답 대기 (REP_SEM P)
    if (semP(semid, REP_SEM) < 0) {
        perror("semP REP_SEM");
        exit(1);
    }

    // 응답 수신
    snprintf(buf, SHM_SIZE, "%s", ptr);
    printf("Received reply: %s\n", buf);

    return 0;
}

