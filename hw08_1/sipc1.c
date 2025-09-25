#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "semlib.h"

/*
 * sipc1.c
 * - 서버 역할을 하는 프로세스
 * - REQ_SEM을 통해 클라이언트의 요청 대기
 * - 요청을 수신하면 처리 후 공유 메모리에 응답 기록
 * - REP_SEM을 통해 응답 준비를 알림
 * - 마지막에 공유 메모리 및 세마포어 제거
 *
 * 변경 사항:
 * 1. busy waiting 제거
 * 2. System V semaphore를 통한 동기화
 * 3. flag를 사용하지 않고 공유 메모리 전부를 데이터 용도로 사용
 * 
 * 동작:
 * - semInit 또는 semGet을 통해 세마포어 획득
 * - REQ_SEM P 연산으로 요청 대기
 * - 요청 문자열 수신 후 응답 문자열 작성
 * - REP_SEM V 연산으로 응답 알림
 * - 작업 완료 후 자원 정리(공유 메모리 제거, 세마포어 제거)
 */

int main()
{
    int shmid, semid;
    char *ptr;
    char buf[SHM_SIZE];

    // 공유 메모리 확보
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT |  SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리 붙이기
    if ((ptr = shmat(shmid, 0, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // 세마포어 접근 (여기서는 생성 역할을 sipc1이 한다 가정)
    // 이미 생성되어 있다면 semGet, 없다면 semInit 사용.
    // 여기서는 예시로 semInit 사용. 실제로는 별도 초기화 코드가 필요할 수 있음.
    if ((semid = semInit(SEM_KEY, 2)) < 0) {
        perror("semInit");
        exit(1);
    }

    // 초기 상태 가정:
    // REQ_SEM: 0 (요청 없음)
    // REP_SEM: 0 (응답 없음)
    // semInit 내에서 이를 0으로 초기화했다고 가정하거나, 별도 초기화 과정 필요.

    // 클라이언트 요청 대기
    // REQ_SEM이 V되기 전까지 대기
    if (semP(semid, REQ_SEM) < 0) {
        perror("semP REQ_SEM");
        exit(1);
    }

    // 요청 수신
    snprintf(buf, SHM_SIZE, "%s", ptr);
    printf("Received request: %s\n", buf);

    // 응답 생성
    snprintf(ptr, SHM_SIZE, "This is a reply from %d.", getpid());
    printf("Replied.\n");

    // 응답 신호 보내기
    if (semV(semid, REP_SEM) < 0) {
        perror("semV REP_SEM");
        exit(1);
    }

    // 응답 전송 후 잠시 대기
    sleep(1);

    // 자원 정리
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl");
        exit(1);
    }

    // 세마포어 제거 (필요 시)
    if (semDestroy(semid) < 0) {
        perror("semDestroy");
        exit(1);
    }

    return 0;
}

