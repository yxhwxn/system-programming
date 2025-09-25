#include <stdio.h>      // 표준 입출력 함수 포함
#include <stdlib.h>     // 일반적인 유틸리티 함수 포함
#include <sys/types.h>  // 시스템 데이터 타입 정의
#include <sys/stat.h>   // 파일 정보 구조체(stat) 포함
#include <unistd.h>     // 다양한 POSIX 함수 포함
#include <pwd.h>        // 사용자 정보를 위한 구조체 포함
#include <grp.h>        // 그룹 정보를 위한 구조체 포함
#include <time.h>       // 시간 관련 함수 포함
#include <dirent.h>     // 디렉토리 입출력 함수 포함

// 파일의 권한을 출력하는 함수 정의
void print_permissions(mode_t mode) {
    // 파일 유형을 결정하여 표시
    char file_type = '?';
    if (S_ISREG(mode)) file_type = '-';     // 일반 파일
    else if (S_ISDIR(mode)) file_type = 'd'; // 디렉토리
    else if (S_ISLNK(mode)) file_type = 'l'; // 심볼릭 링크
    else if (S_ISCHR(mode)) file_type = 'c'; // 문자 장치
    else if (S_ISBLK(mode)) file_type = 'b'; // 블록 장치
    else if (S_ISFIFO(mode)) file_type = 'p'; // FIFO
    else if (S_ISSOCK(mode)) file_type = 's'; // 소켓

    printf("%c", file_type); // 파일 유형 출력

    // 소유자 권한 표시
    printf("%c", (mode & S_IRUSR) ? 'r' : '-'); // 읽기 권한
    printf("%c", (mode & S_IWUSR) ? 'w' : '-'); // 쓰기 권한
    printf("%c", (mode & S_IXUSR) ? 'x' : '-'); // 실행 권한

    // 그룹 권한 표시
    printf("%c", (mode & S_IRGRP) ? 'r' : '-'); // 읽기 권한
    printf("%c", (mode & S_IWGRP) ? 'w' : '-'); // 쓰기 권한
    printf("%c", (mode & S_IXGRP) ? 'x' : '-'); // 실행 권한

    // 다른 사용자 권한 표시
    printf("%c", (mode & S_IROTH) ? 'r' : '-'); // 읽기 권한
    printf("%c", (mode & S_IWOTH) ? 'w' : '-'); // 쓰기 권한
    printf("%c ", (mode & S_IXOTH) ? 'x' : '-'); // 실행 권한
}

// 파일 정보를 출력하는 함수 정의
void print_file_info(const char *filename) {
    struct stat file_stat; // 파일 상태 정보를 저장할 구조체

    // 파일 상태를 가져옴
    if (stat(filename, &file_stat) < 0) {
        perror("stat"); // 오류 메시지 출력
        exit(EXIT_FAILURE);
    }

    // 파일 권한 출력
    print_permissions(file_stat.st_mode);

    // 링크 수 출력
    printf("%ld ", file_stat.st_nlink);

    // 소유자와 그룹 출력
    struct passwd *pwd = getpwuid(file_stat.st_uid); // UID로 사용자 이름 가져오기
    struct group *grp = getgrgid(file_stat.st_gid);  // GID로 그룹 이름 가져오기
    printf("%s %s ", pwd->pw_name, grp->gr_name);    // 사용자명과 그룹명 출력

    // 파일 크기 출력
    printf("%5ld ", file_stat.st_size);

    // 마지막 수정 시간 출력
    char time_str[20];  // 시간 문자열을 저장할 버퍼
    struct tm *timeinfo = localtime(&file_stat.st_mtime); // 수정 시간 로컬 시간으로 변환
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo); // 시간 형식 설정
    printf("%s ", time_str);

    // 파일 이름 출력
    printf("%s\n", filename);
}

// 메인 함수 정의
int main(int argc, char *argv[]) {
    DIR *dir;             // 디렉토리를 가리키는 포인터
    struct dirent *entry; // 디렉토리 엔트리를 저장할 구조체

    // 디렉토리 이름 설정, 인수로 제공된 디렉토리가 없으면 현재 디렉토리를 사용
    const char *dir_name = (argc > 1) ? argv[1] : ".";
    
    // 디렉토리 열기
    dir = opendir(dir_name);
    if (!dir) {
        perror("opendir"); // 오류 메시지 출력
        exit(EXIT_FAILURE);
    }

    // 디렉토리 내의 항목을 읽어옴
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // 숨김 파일은 건너뜀
            print_file_info(entry->d_name); // 파일 정보 출력
        }
    }

    closedir(dir); // 디렉토리 닫기
    return 0;
}

