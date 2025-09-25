#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semlib.h"

/*
 * semlib.c
 * System V 세마포어를 POSIX 스타일 API 비슷하게 제공하기 위한 래퍼 함수들.
 * 
 * 함수 설명:
 * - semInit(key_t key, int nsems): 
 *     주어진 key로 nsems 개의 세마포어 집합을 생성/획득하고, 모두 0으로 초기화한다.
 * - semGet(key_t key, int nsems): 
 *     이미 생성된 세마포어 집합을 가져온다(생성 안하고 접근만).
 * - semP(int semid, int semnum): 
 *     세마포어 집합 semid 중 semnum 번 세마포어에 대해 P 연산(semop) 수행(자원 획득 대기).
 * - semV(int semid, int semnum):
 *     세마포어 집합 semid 중 semnum 번 세마포어에 대해 V 연산(자원 반환, signal).
 * - semDestroy(int semid):
 *     semid에 해당하는 세마포어 집합 제거.
 *
 * 주석:
 * - System V 세마포어는 semctl, semop, semget 사용.
 * - 초기화 시 semctl(..., SETVAL, ...) 또는 semctl(..., SETALL, ...) 이용
 * - 여기서는 모든 세마포어를 0으로 초기화한다고 가정.
 * - 실제 필요에 따라 초기값 변경하거나 세마포어 갯수, 인덱스 운영 필요.
 */

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int semInit(key_t key, int nsems)
{
    int semid;
    union semun arg;
    unsigned short *vals;
    int i;

    // 세마포어 생성 또는 존재하면 접근. IPC_CREAT|IPC_EXCL 로 완전 새로 만드는 것도 가능하지만,
    // 여기서는 단순히 세마포어 없으면 생성되도록 함.
    if ((semid = semget(key, nsems, IPC_CREAT | 0600)) < 0) {
        perror("semget");
        return -1;
    }

    // 모든 세마포어를 0으로 초기화
    vals = (unsigned short *)malloc(sizeof(unsigned short)*nsems);
    if (vals == NULL) {
        perror("malloc");
        return -1;
    }

    for (i = 0; i < nsems; i++)
        vals[i] = 0;

    arg.array = vals;
    if (semctl(semid, 0, SETALL, arg) < 0) {
        perror("semctl(SETALL)");
        free(vals);
        return -1;
    }

    free(vals);
    return semid;
}

int semGet(key_t key, int nsems)
{
    int semid;
    if ((semid = semget(key, nsems, 0)) < 0) {
        perror("semget");
        return -1;
    }
    return semid;
}

int semP(int semid, int semnum)
{
    struct sembuf sops;
    sops.sem_num = semnum;
    sops.sem_op = -1;  // P 연산: 자원을 얻을 때까지 대기
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) < 0) {
        perror("semop P");
        return -1;
    }
    return 0;
}

int semV(int semid, int semnum)
{
    struct sembuf sops;
    sops.sem_num = semnum;
    sops.sem_op = 1;   // V 연산: 자원 반환(신호)
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) < 0) {
        perror("semop V");
        return -1;
    }
    return 0;
}

int semDestroy(int semid)
{
    union semun arg;
    if (semctl(semid, 0, IPC_RMID, arg) < 0) {
        perror("semctl(IPC_RMID)");
        return -1;
    }
    return 0;
}

