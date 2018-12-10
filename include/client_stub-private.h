/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "message.h"
#define MAX_MSG 2048

struct rtable_t{
	char* ip;
	char* port;
	struct sockaddr_in sockaddr;
	int sockfd;
};

 struct thread_params {
	const char *address_port;
	int op;
	int key;
};

struct threads_result {
	int num_put_get;
	double time;
};

/* Funcao que separar uma string em varios tokens separados por
 * seperator. Guarda o numero de tokens em counter
 */
char** tokenizer(char* string, char* seperator, int* counter);