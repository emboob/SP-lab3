/**
 * 시스템 프로그래밍 lab3 과제 실습문제 8번
 * 파일 이름 : filelock.c
 * 만든이 : 20153265 김동현
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd;
    struct flock testlock;
    int pid;

    /* 쓰기 잠금의 인자 지정 */
    testlock.l_type = F_WRLCK;
    testlock.l_whence = SEEK_SET;
    testlock.l_start = 0;
    testlock.l_len = 10;

    /* open file */
    fd = open ("testlock", O_RDWR | O_CREAT, 0666);

    if(fcntl (fd, F_SETLKW, &testlock) == -1) {
        perror("parent : locking");
        exit(1);
    }

    printf("parent: locked record\n");

    pid = fork();

    /* child process */
    if(pid == 0) {
        testlock.l_len = 5;

        if(fcntl (fd, F_SETLKW, &testlock) == -1) {
            printf("child : locking\n");
            exit(1);
        }
        printf("child : locked\n");
        sleep(5);
        printf("child : exiting\n");
    }
    else if(pid > 0) {
        sleep(5);
        printf("parent : exiting\n");
    }
    else
    {
        perror("fork failed");
    }
}
