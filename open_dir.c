/**
 * 시스템 프로그래밍 lab3 과제 실습문제 2번
 * 파일 이름 : open_dir.c
 * 만든이 : 20153265 김동현
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *dirent;

    if(argc < 2)
    {
        fprintf(stderr, "사용 방법 : %s <path name>\n", argv[0]);
        exit(1);
    }

    if((dir = opendir(argv[1])) < 0)
    {
        printf("open failed.\n");
        exit(1);
    }

    // dirent d_name = 파일 이름 , d_ino = 디렉토리 inode 번호
    while((dirent = readdir(dir)) != NULL)
    {
        printf("%s %ld\n", dirent->d_name, dirent->d_ino);
    }

    closedir(dir);

    return 0;
}
