#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>    // fork(), close() 등을 사용하기 위해 필요
#include <stdlib.h>    // exit(), perror() 등을 사용하기 위해 필요
#include <string.h>    // bzero(), memset() 등을 사용하기 위해 필요
#include "tcp.h"       // 사용자 정의 헤더(예: MsgType, SERV_TCP_PORT 상수 등의 정의)

int Sockfd; // 서버 소켓 디스크립터(리스닝 소켓)

// SIGINT(Ctrl+C) 시그널을 받아 서버를 종료하는 함수
void CloseServer()
{
    close(Sockfd); // 서버 소켓 닫기
    printf("\nTCP Server exit.....\n");
    exit(0); // 프로세스 종료
}

int main(int argc, char *argv[])
{
    int                 newSockfd;   // accept 후 만들어지는 커넥션용 소켓 디스크립터
    int                 cliAddrLen;  // 클라이언트 주소 길이를 저장하는 변수
    int                 n;           // read/write 리턴값 저장 용
    struct sockaddr_in  cliAddr;     // 클라이언트 주소 구조체
    struct sockaddr_in  servAddr;    // 서버 주소 구조체
    MsgType             msg;         // 통신에 사용할 메시지 구조체(사용자 정의)
    pid_t               pid;         // fork() 호출 시 반환받을 프로세스 ID

    // SIGINT를 받으면 CloseServer를 실행하도록 시그널 핸들러 설치
    signal(SIGINT, CloseServer);

    // TCP 소켓 생성(PF_INET: IPv4 프로토콜, SOCK_STREAM: TCP)
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));   // servAddr 메모리를 0으로 초기화
    servAddr.sin_family = PF_INET;                // IPv4 사용
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP 인터페이스에서 접속 허용
    servAddr.sin_port = htons(SERV_TCP_PORT);     // tcp.h에서 정의한 서버 포트 번호 사용

    // 서버 소켓에 주소를 bind
    if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }

    // 클라이언트의 접속 요청을 대기하는 상태로 전환 (동시 5개 까지 대기)
    listen(Sockfd, 5);

    printf("TCP Server started.....\n");

    // 클라이언트 주소 길이 변수 설정
    cliAddrLen = sizeof(cliAddr);

    // 무한 루프 돌며 클라이언트 요청을 처리
    while (1)  {
        // 클라이언트 연결 요청 수락
        newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
        if (newSockfd < 0)  {
            perror("accept");
            exit(1);
        }

        // fork()를 통해 자식 프로세스를 생성
        pid = fork();
        if (pid < 0) {
            // fork 실패 시 에러 처리하고, 소켓 닫고 계속 진행
            perror("fork");
            close(newSockfd);
            continue;
        } else if (pid == 0) {
            // 자식 프로세스
            // 부모 프로세스에서 사용중인 리스닝 소켓은 자식에선 필요 없으므로 닫음
            close(Sockfd);

            // 클라이언트로부터 메시지 읽기
            if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
                perror("read");
                exit(1);
            }
            printf("Received request: %s.....", msg.data);

            // 답변 메시지 작성
            msg.type = MSG_REPLY;
            sprintf(msg.data, "This is a reply from %d.", getpid());

            // 클라이언트에 메시지 전송
            if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
                perror("write");
                exit(1);
            }
            printf("Replied.\n");

            // 약간의 딜레이
            usleep(10000);

            // 자식 프로세스에서 사용한 연결 소켓 닫기
            close(newSockfd);

            // 자식 프로세스 종료 (접속 처리 완료)
            exit(0);

        } else {
            // 부모 프로세스
            // 자식 프로세스가 처리하므로 부모에서는 연결 소켓을 닫고 다시 accept()로 돌아감
            close(newSockfd);
        }
    }
    return 0;
}

