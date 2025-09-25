#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 12340          // 서버의 포트 번호
#define BUFFER_SIZE 1024    // 버퍼 크기 정의

// 클라이언트 요청을 처리하는 함수 (스레드 함수)
void* handle_client(void* arg) {
    int client_socket = *(int*)arg;  // 클라이언트 소켓 파일 디스크립터
    free(arg);  // 동적 할당된 메모리 해제

    // char file[];          // 클라이언트로부터 받은 파일
    char buffer[BUFFER_SIZE];

    // 클라이언트로부터 파일 받기
    memset(buffer, 0, BUFFER_SIZE);  // 버퍼 초기화
    read(client_socket, buffer, sizeof(buffer));

	/*
		1. 클라이언트로부터 받은 파일을 fwrite로 저장
		2. 파일의 저장을 마친 후, 파일의 이름을 출력하고 종료
	*/

    close(client_socket);  // 클라이언트 소켓 닫기
    pthread_exit(NULL);    // 스레드 종료
}

int main() {
    int server_socket, client_socket;  // 서버와 클라이언트 소켓
    struct sockaddr_in server_addr, client_addr;  // 주소 구조체
    socklen_t client_len = sizeof(client_addr);   // 클라이언트 주소 길이

    // 1. 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. 서버 주소 설정
    server_addr.sin_family = AF_INET;            // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;    // 모든 네트워크 인터페이스에서 수신
    server_addr.sin_port = htons(PORT);          // 포트를 네트워크 바이트 순서로 변환

    // 3. 소켓과 주소를 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    // 4. 연결 대기 상태 설정
    listen(server_socket, 5);  // 동시에 최대 5개의 클라이언트 연결 대기
    printf("Server is listening on port %d...\n", PORT);

    // 5. 클라이언트 연결 요청 처리 (무한 루프)
    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected\n");

        // 6. 스레드를 사용해 클라이언트 요청 처리
        pthread_t thread;                       // 스레드 ID
        int* new_sock = malloc(sizeof(int));    // 동적 메모리 할당 (소켓 전달)
        *new_sock = client_socket;

        // 스레드 생성 및 실행
        if (pthread_create(&thread, NULL, handle_client, (void*)new_sock) != 0) {
            perror("Thread creation failed");
            close(client_socket);
            free(new_sock);
        }
        pthread_detach(thread);  // 스레드가 종료되면 자원 해제
    }

    close(server_socket);  // 서버 소켓 닫기
    return 0;
}
