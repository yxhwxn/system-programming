#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024  // 버퍼 크기 정의

int main(int argc, char* argv[]) {
    // 명령행 인자 확인
    if (argc != 4) {
        fprintf(stderr, "(Invalid Request)\n	사용법: %s <서버 주소> <포트 번호> <파일명>\n", argv[0]);
		printf("	예시: ./ftpc 127.0.0.1 12340 ftpc.c\n");
        exit(1);
    }

    // 명령행 인자에서 서버 정보와 숫자 추출
    char* server_ip = argv[1];      // 서버 IP 주소
    int port = atoi(argv[2]);       // 서버 포트 번호
    char* file_name = argv[3];     // 클라이언트가 전송할 파일명
    char buffer[BUFFER_SIZE];

    // 1. 소켓 생성
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. 서버 주소 설정
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  // 구조체 초기화
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_port = htons(port);           // 포트를 네트워크 바이트 순서로 변환
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_socket);
        exit(1);
    }

    // 3. 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(1);
    }

    // 4. 파일을 서버에 전송
    snprintf(buffer, sizeof(buffer), "%s", file_name);
    write(client_socket, buffer, strlen(buffer));

    // 5. 서버로부터 결과를 수신
    memset(buffer, 0, sizeof(buffer));  // 버퍼 초기화
    read(client_socket, buffer, sizeof(buffer));

    printf("Server result: %s\n", buffer);  // 서버의 결과 출력

    close(client_socket);  // 소켓 닫기
    return 0;
}
