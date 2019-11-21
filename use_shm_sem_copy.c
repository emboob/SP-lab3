/**
 * 시스템 프로그래밍 lab3 과제 실습문제 10번
 * 파일 이름 : use_shm_sem_copy.c
 * 만든이 : 20153265 김동현
 */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define SEMNAME "/my_sem"
#define SHMSIZE 1024
#define SHMNAME "/my_shm"

void psem_parent(sem_t *semd);
void psem_child(sem_t *semd);
void p(sem_t *semd);
void v(sem_t *semd);

int main(int argc, char *argv[])
{
    int shmd, len;
    int i, val;
    int src_fd, dst_fd;
    
    char ch[SHMSIZE];
    
    sem_t *semd;
    pid_t pid;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    ssize_t read_count;

    void *shmaddr;
    
    if((semd = sem_open(SEMNAME, O_CREAT, 0600, 1)) == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    sem_getvalue(semd, &val);

    if(argc != 3)
    {
        fprintf(stderr, "사용 방법 : ./%s <원본 파일명> <복사할 파일명>\n", argv[0]);
        exit(1);
    }

    pid = fork();

    // fork 실패
    if(pid == -1)
    {
        perror("can't fork\n");
        exit(1);
    }

    // 자식 프로세스
    if(pid == 0)
    {
        p(semd);
        printf("자식 프로세스 시작\n");

        sleep(3);

        if((shmaddr = mmap(0, SHMSIZE, PROT_WRITE, MAP_SHARED, shmd, 0)) == MAP_FAILED)
        {
            perror("mmap_failed");
            exit(1);
        }

        if(ftruncate(shmd, SHMSIZE) != 0)
        {
            perror("ftruncate failed");
            exit(1);
        }

        if(mlock(shmaddr, SHMSIZE) != 0)
        {
            perror("mlock failed");
            exit(1);
        }

        // 파일복사 시작
        if((dst_fd = creat(argv[2], mode)) == -1)
        {
            perror("dst open");
            exit(1);
        }

        while((read_count = read(src_fd, ch, SHMSIZE)) > 0)
        {
            write(dst_fd, shmaddr, read_count);
        }

        if (read_count < 0)
        {
            perror("read");
        }

        close(dst_fd);
        // 끝

        //strcpy((char *)shmaddr, read(dst_fd, ch, read_count));

        if(munmap(shmaddr, SHMSIZE) == -1)
        {
            perror("munmap failed");
            exit(1);
        }
        close(shmd);

        printf("자식 프로세스 종료\n");
        v(semd);
    }
    // 부모 프로세스
    else
    {
        p(semd);
        printf("부모 프로세스 시작\n");

        sleep(3);

        if((shmaddr = mmap(0, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED)
        {
            perror("mmap_failed");
            exit(1);
        }

        if(mlock(shmaddr, SHMSIZE) != 0)
        {
            perror("mlock failed");
            exit(1);
        }

        if((src_fd = open(argv[1], O_RDONLY)) == -1)
        {
            perror("src_open");
            exit(1);
        }

        strcpy((char *)shmaddr, (char *)read(dst_fd, ch, read_count));

        close(src_fd);

        printf("%s", (char*)shmaddr);

        if(munmap(shmaddr, SHMSIZE) == -1)
        {
            perror("munmap failed");
            exit(1);
        }

        close(shmd);

        if (shm_unlink(SHMNAME) == -1)
        {
            perror ("shm_unlink failed");
            exit(1);
        }

        printf("부모 프로세스 종료\n");
        v(semd);
    }

    if(sem_close(semd) == -1) {
        perror("sem_close failed");
        exit(1);
    }
    
    if(sem_unlink(SEMNAME) == -1) {
        perror("sem_unlink failed");
        exit(1);
    }

    return 0;
}

void psem_parent(sem_t *semd)
{
    p(semd);
    printf("process %d : semaphore in use\n", getpid());
    
    printf("process %d : putting semaphore\n", getpid());
    v(semd);
    exit(0);
}

void psem_child(sem_t *semd)
{
    p(semd);
    printf("process %d : semaphore in use\n", getpid());
    sleep(rand()%5);
    printf("process %d : putting semaphore\n", getpid());
    v(semd);
    exit(0);
}

void p(sem_t *semd)
{
    int ret;
    if((ret = sem_trywait(semd)) != 0 && errno == EAGAIN)
        sem_wait(semd);
    else if (ret != 0)
    {
        perror("sem_trywait failed");
        exit(1);
    }
}

void v(sem_t *semd)
{
    if(sem_post(semd) != 0) {
        perror("sem_post failed");
        exit(1);
    }
}
