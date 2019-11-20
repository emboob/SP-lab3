#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>

#define MQ_1 "/mq1"
#define MQ_2 "/mq2"
#define MSG_SIZE 256
#define MAX_MSG 5
#define MAX_LOG 512

pthread_mutex_t  mutex;
int fd;
mqd_t mq1, mq2;
char send_buf [MSG_SIZE];
char recv_buf [MSG_SIZE];
char log_buf [MAX_LOG];
char *ptr = '\0';
time_t ltime;
struct tm *today;
short in_user = 0;

void logg_f(char* str, char* user)
{
	pthread_mutex_lock(&mutex);
	time(&ltime);

	today = localtime(&ltime);
	ptr = asctime(today);
	ptr[strlen(ptr)-1]='\0';

	sprintf(log_buf,"[ %s ] %s : %s \n", ptr, user, str);
	write(fd, log_buf, strlen(log_buf));
	memset(log_buf, '\0', sizeof(log_buf));

	pthread_mutex_unlock(&mutex);
}

void *send_thread(void* args)
{
	while(1)
	{
		memset(send_buf, '\0', sizeof(send_buf));

		printf("> ");

		fgets(send_buf, sizeof(send_buf), stdin);
		send_buf[strlen(send_buf)-1] = '\0';

		if( mq_send(*(mqd_t*)args, send_buf, strlen(send_buf), 0) == -1)
		{
			perror("mq_send()");
		}

		else
		{
			logg_f(send_buf, SEND);

			if(strcmp(send_buf, "/q") == 0)
		  	{
				printf("### Finish Chatt ###\n");
				pthread_mutex_destroy(&mutex);

				close(fd);
				mq_close(mq1);
				mq_close(mq2);

				if(in_user)
				{
					mq_unlink(MQ_1);
					mq_unlink(MQ_2);
				}

				exit(0);
			}

			printf("%s : %s \n", SEND, send_buf);
		}
	}
}

void *recv_thread(void* args)
{
	while(1)
	{
		while(mq_receive(*(mqd_t*)args, recv_buf, sizeof(recv_buf), 0) > 0 )
		{
			logg_f(recv_buf, RECV);
			if( strcmp(recv_buf, "/s") == 0)
			{
				in_user = 0;
				memset(recv_buf, '\0', sizeof(recv_buf));

				break;
			}

		 	if( strcmp(recv_buf, "/q") == 0 )
			{
				in_user = 1;
				printf("Another user is out of here... \n");

				memset(recv_buf, '\0', sizeof(recv_buf));

				break;
			}

			printf("%s : %s\n", RECV, recv_buf);

			memset(recv_buf, '\0', sizeof(recv_buf));
		}
	}
}
