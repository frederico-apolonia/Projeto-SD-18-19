#include <netinet/in.h>
#include "message.h"
#define MAX_MSG 2048

struct rtable_t{
	char* ip;
	char* port;
	struct sockaddr_in sockaddr;
	int sockfd;
};
