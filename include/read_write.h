#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include <unistd.h>
#include <errno.h>

/* Lê todos os bytes do socket */
int read_all (int socket, char** buf);
/* Escreve todos so bytes no socket */
int write_all (int socket, char* buf, int length);

#endif