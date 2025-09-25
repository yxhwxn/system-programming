/*
chats.c를 select 시스템 콜을 사용하여 멀티플렉싱 방식으로 처리하도록 수정
-> thread 대신 단일 프로세스가 여러 클라이언트의 소켓 상태를 감시하면서 I/O를 처리함
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include "chat.h"

#define MAX_CLIENT 5        // 최대 클라이언트 수
#define MAX_ID 32           // 클라이언트 ID 최대 길이
#define MAX_BUF 256         // 메시지 버퍼 최대 길이

// 클라이언트 정보를 저장하는 구조체
typedef struct {
    int sockfd;       // 클라이언트 소켓 파일 디스크립터
    int inUse;        // 클라이언트 활성 상태 (1: 사용 중, 0: 비활성)
    char uid[MAX_ID]; // 클라이언트의 ID
} ClientType;

// 전역 변수
int Sockfd;                     // 서버 소켓
ClientType Client[MAX_CLIENT];  // 클라이언트 목록
fd_set readfds;                 // `select` 감시용 fd_set
int max_fd;                     // 현재 가장 큰 파일 디스크립터 값

// 클라이언트로부터의 메시지를 다른 클라이언트로 전송
void SendToOtherClients(int sender_id, char *buf) {
    char msg[MAX_BUF + MAX_ID];  // 전송할 메시지 버퍼
    sprintf(msg, "%s> %s", Client[sender_id].uid, buf);  // 보낸 클라이언트 ID와 메시지 조합

    printf("%s", msg);  // 서버에 메시지 출력 (디버깅 용도)
    fflush(stdout);

    // 모든 활성 클라이언트에게 메시지 전송
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse && i != sender_id) {  // 활성 상태 & 보낸 클라이언트 제외
            if (send(Client[i].sockfd, msg, strlen(msg) + 1, 0) < 0) {
                perror("send");  // 메시지 전송 실패 처리
            }
        }
    }
}

// 서버 종료 시 모든 자원 해제
void CloseServer(int signo) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse) {
            close(Client[i].sockfd);  // 클라이언트 소켓 닫기
        }
    }
    close(Sockfd);  // 서버 소켓 닫기
    printf("\nChat server terminated...\n");
    exit(0);
}

// 클라이언트를 추가하고 소켓을 관리
int AddClient(int sockfd) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!Client[i].inUse) {  // 비활성 클라이언트 슬롯 찾기
            Client[i].sockfd = sockfd;  // 소켓 저장
            Client[i].inUse = 1;        // 활성 상태 설정
            bzero(Client[i].uid, MAX_ID);  // 클라이언트 ID 초기화
            return i;  // 클라이언트 ID 반환
        }
    }
    return -1;  // 실패 (최대 클라이언트 수 초과)
}

// 클라이언트를 제거하고 소켓 정리
void RemoveClient(int id) {
    close(Client[id].sockfd);  // 클라이언트 소켓 닫기
    Client[id].inUse = 0;      // 비활성 상태로 설정
    printf("Client %d disconnected...\n", id);
}

int main(int argc, char *argv[]) {
    int cli_sockfd, cli_addr_len, id, n;
    struct sockaddr_in cliAddr, servAddr;  // 클라이언트, 서버 주소 구조체
    char buf[MAX_BUF];                     // 메시지 버퍼

    // 서버 종료 시그널 핸들러 등록
    signal(SIGINT, CloseServer);

    // 서버 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // 주소 재사용 옵션 설정
    int opt = 1;
    setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 서버 주소 설정
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;              // IPv4 사용
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 모든 네트워크 인터페이스 수신
    servAddr.sin_port = htons(SERV_TCP_PORT);   // 서버 포트 설정

    // 소켓과 주소를 바인딩
    if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 리스닝 상태로 전환
    listen(Sockfd, 5);
    printf("Chat server started...\n");

    // 클라이언트 초기화
    for (int i = 0; i < MAX_CLIENT; i++) {
        Client[i].inUse = 0;  // 모든 클라이언트를 비활성 상태로 초기화
    }

    // `select` 준비
    max_fd = Sockfd;  // 초기 max_fd는 서버 소켓
    FD_ZERO(&readfds);  // fd_set 초기화

    while (1) {
        fd_set temp_fds = readfds;  // 임시 fd_set 복사
        FD_SET(Sockfd, &temp_fds);  // 서버 소켓을 fd_set에 추가

        // 활성 클라이언트 소켓을 fd_set에 추가
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (Client[i].inUse) {
                FD_SET(Client[i].sockfd, &temp_fds);
                if (Client[i].sockfd > max_fd) {
                    max_fd = Client[i].sockfd;  // max_fd 갱신
                }
            }
        }

        // `select` 호출
        if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            continue;
        }

        // 새 클라이언트 연결 처리
        if (FD_ISSET(Sockfd, &temp_fds)) {
            cli_addr_len = sizeof(cliAddr);
            cli_sockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cli_addr_len);
            if (cli_sockfd < 0) {
                perror("accept");
                continue;
            }

            id = AddClient(cli_sockfd);
            if (id < 0) {
                printf("Max clients reached. Connection refused.\n");
                close(cli_sockfd);
            } else {
                printf("Client %d connected...\n", id);
            }
        }

        // 클라이언트로부터 메시지 수신
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (Client[i].inUse && FD_ISSET(Client[i].sockfd, &temp_fds)) {
                n = recv(Client[i].sockfd, buf, MAX_BUF, 0);
                if (n <= 0) {  // 클라이언트 연결 종료
                    RemoveClient(i);
                } else if (strlen(Client[i].uid) == 0) {  // 처음 받은 메시지는 클라이언트 ID
                    strncpy(Client[i].uid, buf, MAX_ID);
                    printf("Client %d logged in (ID: %s)\n", i, Client[i].uid);
                } else {  // 클라이언트의 메시지 처리
                    SendToOtherClients(i, buf);
                }
            }
        }
    }
    return 0;
}

