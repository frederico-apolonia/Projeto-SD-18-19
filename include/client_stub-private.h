/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <netinet/in.h>
#include "message.h"
#define MAX_MSG 2048

struct rtable_t{
	char* ip;
	char* port;
	struct sockaddr_in sockaddr;
	int sockfd;
};

/* Funcao que separar uma string em varios tokens separados por
 * seperator. Guarda o numero de tokens em counter
 */
char** tokenizer(char* string, char* seperator, int* counter);