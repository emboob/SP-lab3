/**
 * 시스템 프로그래밍 lab3 과제 실습문제 5번
 * 파일 이름 : process_fork.c
 * 만든이 : 20153265 김동현
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int i;
    int num;

    if(argc < 1)
    {
        printf("hello\n");
    }
    else
    {
        num = atoi(argv[1]);
        printf("hello");

        for(i=0; i<num; i++)
            fork();

        printf("\n");
    }

    return 0;
}
