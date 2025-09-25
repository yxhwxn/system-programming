#ifndef _SHM_H_
#define _SHM_H_

#define SHM_KEY 0x1234
#define SHM_SIZE 1024
#define SHM_MODE 0600  // 사용자 읽기/쓰기 권한

#define SEM_KEY 0x5678
// 세마포어 개수 2개: 0 -> REQ_SEM, 1 -> REP_SEM
#define REQ_SEM 0
#define REP_SEM 1

#endif

