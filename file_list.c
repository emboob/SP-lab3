/**
 * 시스템 프로그래밍 lab3 과제 실습문제 6번
 * 파일 이름 : file_list.c
 * 만든이 : 20153265 김동현
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <string.h>

int total_size = 0;

char *uid(uid_t uid)
{
    return getpwuid(uid)->pw_name;
}

char *gid(gid_t gid)
{
    return getgrgid(gid)->gr_name;
}

char *ch_time(time_t *timep)
{
    return ctime(timep);
}

void inf(char *file)
{ 
    struct stat st;
    stat(file, &st);
    char mod[20];

    strcpy(mod,"----------");
    if ((st.st_mode & S_IFMT) == S_IFDIR) {
        mod[0]='d';
    }
    
    // 관리자
    if (st.st_mode & S_IRUSR) mod[1] = 'r';
    if (st.st_mode & S_IWUSR) mod[2] = 'w';
    if (st.st_mode & S_IXUSR) mod[3] = 'x';
    // 소유자
    if (st.st_mode & S_IRGRP) mod[4] = 'r';
    if (st.st_mode & S_IWGRP) mod[5] = 'w';
    if (st.st_mode & S_IXGRP) mod[6] = 'x';
    // 사용자
    if (st.st_mode & S_IROTH) mod[7] = 'r';
    if (st.st_mode & S_IWOTH) mod[8] = 'w';
    if (st.st_mode & S_IXOTH) mod[9] = 'x';

    printf("%s ", mod);
    printf("%3d ", st.st_nlink);
    printf("%s ", uid(st.st_uid));
    printf("%s ", gid(st.st_gid));
    printf("%d ", st.st_size);
    total_size += st.st_size;
    
    char *p = ch_time(&st.st_ctime)+4;
    for (int i = 0; *p != '\n'; i++) {
        printf("%c", *p);
        p++;
    }
    printf(" ");
    if(mod[0] == 'd') {
        printf("\033[1;34m%s\033[0m\n", file);
    } else if (mod[9] == 'x'){
        printf("\033[1;32m%s\033[0m\n", file);
    } else {
        printf("%s\n", file);
    }
}

void ls(char *dir)
{
    struct dirent *drtp;
    DIR *dp = opendir(dir);

    while (drtp = readdir(dp)) {
        inf(drtp -> d_name);
    }

    closedir(dp);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        ls(".");
    }
    else
    {
        ++argv;
        ls(*argv);
    }
    printf("total %d\n", total_size / 1024);

    return 0;
}
