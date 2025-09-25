#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>     // pthread_t, pthread_create() 등을 사용하기 위해 필요
#include <stdlib.h>      // exit(), perror() 등을 사용하기 위해 필요
#include <string.h>      // memset, bzero 등 사용하기 위해 필요
#include "select.h"      // MsgType, SERV_TCP_PORT 등 정의

int TcpSockfd; // TCP 소켓 디스크립터(리스닝 소켓)
int UdpSockfd; // UDP 소켓 디스크립터
int UcoSockfd; // UNIX domain 연결지향 소켓 디스크립터(리스닝 소켓)
int UclSockfd; // UNIX domain 비연결지향 소켓 디스크립터

// SIGINT(Ctrl+C)가 발생할 경우 서버를 종료하는 함수
void CloseServer()
{
    close(TcpSockfd);
    close(UdpSockfd);
    close(UcoSockfd);
    close(UclSockfd);
    if (remove(UNIX_STR_PATH) < 0)  {
        perror("remove");
    }
    if (remove(UNIX_DG_PATH) < 0)  {
        perror("remove");
    }

    printf("\nServer daemon exit.....\n");
    exit(0);
}

// TCP 소켓 생성 및 bind, listen 설정
void MakeTcpSocket()
{
    struct sockaddr_in  servAddr;

    // TCP 소켓 생성 (IPv4, TCP)
    if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;   
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP 인터페이스에서 접속 허용
    servAddr.sin_port = htons(SERV_TCP_PORT);     // tcp.h에 정의된 TCP 포트 사용

    // 소켓과 서버 주소 bind
    if (bind(TcpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }

    // 클라이언트 접속 대기(동시 5개)
    listen(TcpSockfd, 5);
}

// UDP 소켓 생성 및 bind 설정
void MakeUdpSocket()
{
    struct sockaddr_in  servAddr;

    // UDP 소켓 생성 (IPv4, UDP)
    if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;   
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_UDP_PORT); // udp.h에 정의된 UDP 포트 사용 (select.h 내에 정의)

    // 소켓과 서버 주소 bind
    if (bind(UdpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }
}

// UNIX Domain 소켓 (Connection-Oriented) 생성 및 bind, listen 설정
void MakeUcoSocket()
{
    struct sockaddr_un  servAddr;
    int                 servAddrLen;

    // UNIX domain 소켓 생성 (스트림, TCP 유사)
    if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // UNIX domain 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_STR_PATH);
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    // 소켓과 UNIX domain 주소 bind
    if (bind(UcoSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
        perror("bind");
        exit(1);
    }

    // 클라이언트 접속 대기
    listen(UcoSockfd, 5);
}

// UNIX Domain 소켓 (Connection-Less) 생성 및 bind 설정
void MakeUclSocket()
{
    struct sockaddr_un  servAddr;
    int                 servAddrLen;

    // UNIX domain 소켓 생성 (데이터그램, UDP 유사)
    if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // UNIX domain 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_DG_PATH);
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    // 소켓과 UNIX domain 주소 bind
    if (bind(UclSockfd, (struct sockaddr *)&servAddr, servAddrLen) < 0)  {
        perror("bind");
        exit(1);
    }
}

// TCP 요청을 처리하는 스레드 함수
// TCP는 connection-oriented 소켓이므로 accept 후 read/write로 데이터 교환
void *TcpThread(void *arg)
{
    int                 newSockfd, cliAddrLen, n;
    struct sockaddr_in  cliAddr;
    MsgType             msg;

    while (1) {
        cliAddrLen = sizeof(cliAddr);
        // 클라이언트 접속 수락 - blocking 상태로 대기
        newSockfd = accept(TcpSockfd, (struct sockaddr *)&cliAddr, (socklen_t *)&cliAddrLen);
        if (newSockfd < 0)  {
            perror("accept");
            continue; 
        }

        // 메시지 수신
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
            perror("read");
            close(newSockfd);
            continue;
        }
        printf("Received TCP request: %s.....", msg.data);

        // 응답 메시지 전송
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
            perror("write");
        }
        printf("Replied.\n");

        close(newSockfd); // 연결 종료
    }
    return NULL;
}

// UDP 요청을 처리하는 스레드 함수
// UDP는 connectionless이므로 recvfrom()/sendto()를 이용
void *UdpThread(void *arg)
{
    int                 cliAddrLen, n;
    struct sockaddr_in  cliAddr;
    MsgType             msg;

    while (1) {
        cliAddrLen = sizeof(cliAddr);
        // 클라이언트 메시지 수신 - blocking 상태로 대기
        if ((n = recvfrom(UdpSockfd, (char *)&msg, sizeof(msg),
                          0, (struct sockaddr *)&cliAddr, (socklen_t *)&cliAddrLen)) < 0) {
            perror("recvfrom");
            continue;
        }
        printf("Received UDP request: %s.....", msg.data);

        // 응답 메시지 전송
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (sendto(UdpSockfd, (char *)&msg, sizeof(msg),
                   0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
            perror("sendto");
        }
        printf("Replied.\n");
    }
    return NULL;
}

// UNIX-domain CO(Connection-Oriented) 요청을 처리하는 스레드 함수
// TCP와 유사하게 accept(), read(), write() 사용
void *UcoThread(void *arg)
{
    int                 newSockfd, cliAddrLen, n;
    struct sockaddr_un  cliAddr;
    MsgType             msg;

    while (1) {
        cliAddrLen = sizeof(cliAddr);
        // 클라이언트 접속 수락
        newSockfd = accept(UcoSockfd, (struct sockaddr *)&cliAddr, (socklen_t *)&cliAddrLen);
        if (newSockfd < 0)  {
            perror("accept");
            continue;
        }

        // 메시지 수신
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
            perror("read");
            close(newSockfd);
            continue;
        }
        printf("Received UNIX-domain CO request: %s.....", msg.data);

        // 응답 메시지 전송
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
            perror("write");
        }
        printf("Replied.\n");

        close(newSockfd); // 연결 종료
    }
    return NULL;
}

// UNIX-domain CL(Connection-Less) 요청을 처리하는 스레드 함수
// UDP 방식과 유사하게 recvfrom(), sendto() 사용
void *UclThread(void *arg)
{
    int                 cliAddrLen, n;
    struct sockaddr_un  cliAddr;
    MsgType             msg;

    while (1) {
        cliAddrLen = sizeof(cliAddr);
        // 클라이언트 메시지 수신 - blocking 상태 대기
        if ((n = recvfrom(UclSockfd, (char *)&msg, sizeof(msg),
                          0, (struct sockaddr *)&cliAddr, (socklen_t *)&cliAddrLen)) < 0) {
            perror("recvfrom");
            continue;
        }
        printf("Received UNIX-domain CL request: %s.....", msg.data);

        // 응답 메시지 전송
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (sendto(UclSockfd, (char *)&msg, sizeof(msg),
                   0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
            perror("sendto");
        }
        printf("Replied.\n");
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_t tid_tcp, tid_udp, tid_uco, tid_ucl;

    // SIGINT 발생 시 CloseServer 호출
    signal(SIGINT, CloseServer);

    // 4종류의 소켓 생성
    MakeTcpSocket();
    MakeUdpSocket();
    MakeUcoSocket();
    MakeUclSocket();

    printf("Server daemon started.....\n");

    // 각 소켓을 처리하는 전용 스레드 생성
    if (pthread_create(&tid_tcp, NULL, TcpThread, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid_udp, NULL, UdpThread, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid_uco, NULL, UcoThread, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&tid_ucl, NULL, UclThread, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // 메인 스레드는 여기서 별도 로직 없이 시그널 대기
    // 필요하다면 pthread_join으로 스레드 종료를 기다릴 수도 있다.
    while (1) {
        pause(); // 신호 대기 (SIGINT 등)
    }

    return 0;
}

