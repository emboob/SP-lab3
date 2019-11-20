/**
 * 시스템 프로그래밍 lab3 과제 실습문제 3번
 * 파일 이름 : print_all_file.c
 * 만든이 : 20153265 김동현
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void read_dir(DIR *dir, char *file)
{
    DIR *temp_dir;
    struct dirent *directory;
    struct stat file_state;
    char buff[1024];

    while(directory = readdir(dir))
    {
        if(!strncmp(directory->d_name, ".", 1) || !strcmp(directory->d_name, ".."))
        {
            continue;
        }

        if(directory->d_ino == 0)
        {
            continue;
        }
        
        sprintf(buff, "%s/%s", file, directory->d_name);
        puts(buff);

        if(lstat(buff, &file_state) < 0)
        {
            perror("stat");
            exit(1);
        }
        
        // 디렉토리인지를 검사하는데, 재귀함수를 사용하여 해당 내용을 읽어들임.
        if(S_ISDIR(file_state.st_mode))
        {
            temp_dir = opendir(buff);
            // 재귀함수 사용
            read_dir(temp_dir, buff);
            printf("\n");
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    DIR *dir;
    char file[1024];
    struct dirent *directory;

    if(argc == 1)
    {
        strcpy(file, ".");
    }
    else
    {
        strcpy(file, argv[1]);
    }

    if((dir = opendir(file)) == NULL)
    {
        printf("open failed.");
        exit(1);
    }

    read_dir(dir, file);
}
