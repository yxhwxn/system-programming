/*
chatc.c를 select 시스템 콜 대신 멀티스레드를 사용해 구현한 코드입니다.
주요 변경사항:

수신 전용 스레드: 서버로부터 메시지를 계속 수신합니다.
메인 스레드: 사용자의 입력을 처리하고 서버로 전송합니다.
select 시스템 콜을 제거하고 pthread를 사용해 동시에 입출력을 처리합니다.
*/

/*
주요 변경사항 설명
ReceiveMessages 함수

서버로부터 메시지를 수신하는 스레드 함수입니다.
무한 루프를 통해 recv()로 데이터를 읽어 출력합니다.
서버가 종료되면 소켓을 닫고 프로그램을 종료합니다.
pthread_create()

수신 전용 스레드를 생성합니다.
수신 스레드는 ReceiveMessages()를 실행합니다.
메인 스레드

사용자 입력을 받아 서버에 전송합니다.
사용자 입력이 끝날 때까지 루프를 반복합니다.
SIGINT 처리

Ctrl+C 입력 시 CloseClient() 함수가 호출되어 소켓을 닫고 프로그램을 종료합니다.
스레드 종료

메인 스레드가 종료되기 전에 pthread_cancel()을 사용해 수신 스레드를 종료합니다.
pthread_join()으로 스레드 자원을 정리합니다.
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <ctype.h> // isdigit 함수 선언 포함
#include "chat.h"

#define MAX_BUF 256

int Sockfd;  // 서버와의 소켓 파일 디스크립터

// 서버로부터 메시지를 수신하는 스레드 함수
void* ReceiveMessages(void* arg) {
    char buf[MAX_BUF];
    int n;

    while (1) {
        // 서버로부터 메시지 수신
        if ((n = recv(Sockfd, buf, MAX_BUF, 0)) < 0) {
            perror("recv");
            pthread_exit(NULL);
        }
        if (n == 0) {
            fprintf(stderr, "Server terminated.....\n");
            close(Sockfd);
            exit(1);
        }

        // 받은 메시지를 출력
        printf("%s", buf);
        fflush(stdout);
    }
    return NULL;
}

// SIGINT 시그널 핸들러 (Ctrl+C 처리)
void CloseClient(int signo) {
    close(Sockfd);
    printf("\nChat client terminated.....\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    struct sockaddr_in servAddr;
    struct hostent* hp;
    pthread_t recvThread;  // 수신 전용 스레드

    // 실행 인자 확인
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
        exit(1);
    }

    // 서버와 통신할 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // 서버 주소 설정
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(SERV_TCP_PORT);

    if (isdigit(argv[1][0])) {
        servAddr.sin_addr.s_addr = inet_addr(argv[1]);  // IP 주소 직접 입력
    } else {
        if ((hp = gethostbyname(argv[1])) == NULL) {  // 도메인 이름으로 주소 검색
            fprintf(stderr, "Unknown host: %s\n", argv[1]);
            exit(1);
        }
        memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
    }

    // 서버에 연결
    if (connect(Sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        exit(1);
    }

    // SIGINT 시그널 핸들러 설정
    signal(SIGINT, CloseClient);

    // 클라이언트 ID 입력
    char buf[MAX_BUF];
    printf("Enter ID: ");
    fflush(stdout);
    fgets(buf, MAX_BUF, stdin);
    *strchr(buf, '\n') = '\0';

    // 서버에 ID 전송
    if (send(Sockfd, buf, strlen(buf) + 1, 0) < 0) {
        perror("send");
        exit(1);
    }
    printf("Press ^C to exit\n");

    // 수신 스레드 생성
    if (pthread_create(&recvThread, NULL, ReceiveMessages, NULL) != 0) {
        perror("pthread_create");
        close(Sockfd);
        exit(1);
    }

    // 메인 스레드: 사용자 입력을 서버에 전송
    while (1) {
        fgets(buf, MAX_BUF, stdin);  // 사용자 입력 받기
        if (send(Sockfd, buf, strlen(buf) + 1, 0) < 0) {
            perror("send");
            break;
        }
    }

    // 종료 처리
    pthread_cancel(recvThread);  // 수신 스레드 종료
    pthread_join(recvThread, NULL);
    close(Sockfd);
    return 0;
}

