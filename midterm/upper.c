#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // 파일 포인터 선언
    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        perror("Error opening input file"); // 입력 파일 열기 실패 시 오류 메시지 출력
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "w");
    if (outputFile == NULL) {
        perror("Error opening output file"); // 출력 파일 열기 실패 시 오류 메시지 출력
        fclose(inputFile); // 입력 파일 닫기
        return 1;
    }

    // 파일의 각 문자를 읽어서 대문자로 변환 후 출력 파일에 작성
    char ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        fputc(toupper((unsigned char)ch), outputFile);
    }

    // 파일 닫기
    fclose(inputFile);
    fclose(outputFile);

    printf("File has been converted to uppercase and saved as '%s'.\n", argv[2]);

    return 0;
}

