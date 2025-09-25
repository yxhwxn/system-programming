#include <stdio.h>
#include <ctype.h>

// 대소문자 구분 없이 두 문자열을 비교하는 함수
int my_strcasecmp(const char *str1, const char *str2) {
    
    while (*str1 && *str2) {
        // 소문자로 변환
        char c1 = tolower((unsigned char)*str1);
        char c2 = tolower((unsigned char)*str2);

        // 변환된 문자가 다르면 그 차이를 반환
        if (c1 != c2) {
            printf("Difference at first mismatch: %d (between '%c' and '%c')\n", c1 - c2, *str1, *str2);
            return c1 - c2;
        }

        str1++;
        str2++;
    }

    // 문자열의 길이가 다를 경우 남은 문자 비교
    int final_diff = tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
    if (final_diff != 0) {
        printf("Difference: %d (between '%c' and '%c')\n", final_diff, *str1, *str2);
    }
    return final_diff;
}

int main(int argc, char *argv[]) {
    // 명령줄 인자로 두 개의 문자열이 입력되었는지 확인
    if (argc != 3) {
        printf("Usage: %s <string1> <string2>\n", argv[0]);
        return 1; // 프로그램 비정상 종료
    }

    // 명령줄 인자로부터 문자열을 가져옴
    const char *str1 = argv[1];
    const char *str2 = argv[2];

    // 비교 결과에 따라 출력
    int result = my_strcasecmp(str1, str2);

    if (result == 0) {
        printf("The strings '%s' and '%s' are equal (case-insensitive).\n", str1, str2);
    } else if (result < 0) {
        printf("'%s' is less than '%s' (case-insensitive).\n", str1, str2);
    } else {
        printf("'%s' is greater than '%s' (case-insensitive).\n", str1, str2);
    }

    return 0;
}

