#include <stdio.h>

// 포인터를 사용한 strlen 함수: 문자열의 길이를 반환
int strlen_p(char *str)
{
	int len = 0;
	while (*str++)  {  // 문자열의 끝을 '\0'로 확인하여 문자열 길이를 증가
		len++;
	}
	return len;
}

// 배열을 사용한 strlen 함수: 문자열의 길이를 반환
int strlen_a(char str[])
{
	int i;
	for (i = 0 ; str[i] != '\0' ; i++)  
		;  // 루프는 문자열의 끝 '\0'을 만날 때까지 반복, 별도의 실행 코드 없음
	return i;
}

// 포인터를 사용한 strcpy 함수: src 문자열을 dst에 복사
void strcpy_p(char *dst, char *src)
{
	while (*src)  {  // src 문자열 끝까지 복사
		*dst++ = *src++;
	}
	*dst = *src;  // 마지막에 '\0' 추가
}

// 배열을 사용한 strcpy 함수: src 문자열을 dst에 복사
void strcpy_a(char dst[], char src[])
{
	int i;
	for (i = 0 ; src[i] != '\0' ; i++)  
		dst[i] = src[i];  // 루프는 src 문자열의 끝까지 dst에 복사
	dst[i] = src[i];  // 마지막에 '\0' 추가
}

// 포인터를 사용한 strcat 함수: src 문자열을 dst 문자열 뒤에 연결
void strcat_p(char *dst, char *src)
{
	while (*dst++)  // dst 문자열 끝으로 이동
		;
	dst--;  // null 문자 위치로 돌아감
	while (*src)  {  // src 문자열을 dst 뒤에 추가
		*dst++ = *src++;
	}
	*dst = *src;  // 마지막에 '\0' 추가
}

// 배열을 사용한 strcat 함수: src 문자열을 dst 문자열 뒤에 연결
void strcat_a(char dst[], char src[])
{
	int i, j;
	for (i = 0 ; dst[i] != '\0' ; i++)  
		;  // 루프는 dst 문자열의 끝 '\0'을 만날 때까지 반복, 별도의 실행 코드 없음
	for (j = 0 ; src[j] != '\0' ; j++)  
		dst[i+j] = src[j];  // src 문자열을 dst 뒤에 추가
	dst[i+j] = src[j];  // 마지막에 '\0' 추가
}

// 포인터를 사용한 strcmp 함수: 두 문자열을 비교
int strcmp_p(char *dst, char *src)
{
	while (*dst && (*dst == *src)) {  // 각 문자 비교
		dst++;
		src++;
	}
	// 두 문자열의 차이 반환 (같으면 0)
	return *(unsigned char *)dst - *(unsigned char *)src;
}

// 배열을 사용한 strcmp 함수: 두 문자열을 비교
int strcmp_a(char dst[], char src[])
{
	int i;
	for (i = 0; dst[i] == src[i]; i++) {
		if (dst[i] == '\0')  // 문자열 끝에 도달하면 종료
			return 0;
	}
	// 두 문자열의 차이 반환 (같으면 0)
	return (unsigned char)dst[i] - (unsigned char)src[i];
}

int main()
{
        int len1, len2, cmp1, cmp2;
        char str1[20], str2[20];

        // strlen 테스트
        len1 = strlen_p("Hello");
        len2 = strlen_a("Hello");
        printf("==== strlen 테스트 ====\n");
        printf("strlen_p(\"Hello\"): %d\n", len1);
        printf("strlen_a(\"Hello\"): %d\n", len2);

        // strcpy 테스트
        strcpy_p(str1, "Hello");
        strcpy_a(str2, "Hello");
        printf("\n==== strcpy 테스트 ====\n");
        printf("strcpy_p(\"Hello\"): %s\n", str1);
        printf("strcpy_a(\"Hello\"): %s\n", str2);

        // strcat 테스트
        strcat_p(str1, ", World!");
        strcat_a(str2, ", World!");
        printf("\n==== strcat 테스트 ====\n");
        printf("strcat_p: %s\n", str1);
        printf("strcat_a: %s\n", str2);

        // strcmp 테스트 (같은 문자열 비교)
        cmp1 = strcmp_p("Yuhwan", "Yuhwan");
        cmp2 = strcmp_a("Yuhwan", "Yuhwan");
        printf("\n==== strcmp 테스트 (같은 문자열) ====\n");
        printf("비교할 문자열: \"Yuhwan\" vs \"Yuhwan\"\n");
        printf("strcmp_p 결과: %d (같으면 0)\n", cmp1);
        printf("strcmp_a 결과: %d (같으면 0)\n", cmp2);

        // strcmp 테스트 (다른 문자열 비교)
        cmp1 = strcmp_p("Yuhwan", "Hwang");
        cmp2 = strcmp_a("Yuhwan", "Hwang");
        printf("\n==== strcmp 테스트 (다른 문자열) ====\n");
        printf("비교할 문자열: \"Yuhwan\" vs \"Hwang\"\n");
        printf("strcmp_p 결과: %d (첫 번째로 다른 문자의 ASCII 값 차이)\n", cmp1);
        printf("strcmp_a 결과: %d (첫 번째로 다른 문자의 ASCII 값 차이)\n", cmp2);

        return 0;
}
