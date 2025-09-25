### hw01

> 오픈소스 CMake 분석

### hw02

> Quick Introduction to C
  
- string.c에 다음의 함수를 추가하고, main함수에 사용 예를 보일 것
- int strcmp_p(char *dst, char *src);
- int strcmp_a(char dst[], char src[]);
- 위 두함수는 strcmp 함수와 같은 일을 수행함
 
### hw03

> File I/O

### hw04

> Files & Directories

- "ls -l" 과 유사하게 동작하는 프로그램 mylsl을 작성하시오.
- $ ./mylsl

### hw05

> Processes and Threads

- int mysystem(char *cmd) 함수 구현하시오.
- 동작은 "system" system call과 동일해야 하며, fork, execv, waitpid, system call을 이용하여 구현한다.
- 1부터 100까지의 합을 구하는 프로그램 tadd를 작성하시오. 두개의 thread를 생성하여 각각 1~50, 51~100까지의 합을 구하고, main thread가 각 thread가 종료되기를 기다린 후 각각의 합을 더하여 출력해야 함


### hw06

> Signals

- Parent process가 child process의 종료시, SIGCHLD signal을 asynchronous하게 처리하는 프로그램을 작성
- 즉, child process의 종료를 synchronous하게 처리하기 위해서는 wait() system call을 사용하고, asynchronous하게 처리하기 위해서는 signal을 사용함을 익힘
- [Hint] Processes and Threads의 wait.c 실습 내용에서 wait 부분을 while (1); 로 변경하고, fork() 이전에 signal handler를 등록

### hw07

> IPC

- Inter-Process Communication (1) 
  - pipe.c 에 추가하여 child도 parent에게 메시지를 전달하도록 변경
- Inter-Process Communication (2)
  - mipc.c 를 shared memory system call을 사용하도록 변경
  - [Hint] open, mmap 대신 shmget, shmat를 사용

### hw08

> Synchronization

1. IPC 실습내용 중 sipc1.c sipc2.c를 다음과 같이 수정
- busy waiting을 없애고, semaphore를 통해 동기화시킴
- buisy waiting을 위해 사용했던 flag 4 bytes를 제외하고, shared memory 부분은 모두 데이터 전달을 위해 사용
- [Hint] Process간 동기화 이므로, System V semaphore를 사용해야 하며, POSIX semaphore API처럼 만든 semlib.c의 함수를 사용하면 됨

2. IPC 실습내용 중 sipc1.c sipc2.c를 다음과 같이 수정
- Multi-thread version으로 변경
- 이때, shared memory를 없애고, 전역변수로 설정
  1. 전역변수로 메시지를 주고 받을때, mutex와 condition variable을 사용 
  2. semaphore를 사용
- 1과 2의 방법을 비교


### hw09

> Socket

- tcps.c를 connection이 맺어진 후 process를 생성하여 전담처리하도록 변경
- select.c를 thread version으로 변경
