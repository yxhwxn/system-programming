#ifndef _SEMLIB_H_
#define _SEMLIB_H_

int semInit(key_t key, int nsems); // semget()를 통한 세마포어 생성 및 초기화
int semGet(key_t key, int nsems);  // 이미 생성된 세마포어 접근
int semP(int semid, int semnum);   // P 연산 (semop) - 감소 연산
int semV(int semid, int semnum);   // V 연산 (semop) - 증가 연산
int semDestroy(int semid);         // 세마포어 제거

#endif

