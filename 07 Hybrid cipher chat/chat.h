#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <openssl/des.h>

#define NAME_SIZE	64
#define MSG_SIZE	256

pthread_mutex_t mutex;

typedef struct {
	char name[NAME_SIZE];
	char msg[MSG_SIZE];
} TALK;

void handler(void)
{
	fprintf(stdout, "\n\033[F\033[J");
	fprintf(stdout, "-------------------------------------\n");
	fprintf(stdout, ">> EXIT \n");
	fprintf(stdout, "-------------------------------------\n");
	exit(1);
}
