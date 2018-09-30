#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"

struct node_t {
	struct entry_t *elem;
	struct node_t *next;

};

struct list_t {
	struct node_t *head;
	struct node_t * tail;
};

void list_print(struct list_t* list);
/*
 * Funcao que percorre uma estrutura de nodes, quando encontrar um
 * node a  NULL insere neste valor uma nova struct node_t com o 
 * atributo da entry_t inicializado
 */
int list_add_node(struct node_t *noactual,struct entry_t *entryAPor);
#endif
