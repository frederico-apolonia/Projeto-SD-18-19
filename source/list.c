#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list-private.h"

struct list_t *list_create(){
	struct node_t *cabeca = (struct node_t*) malloc(sizeof(struct node_t));
	struct list_t *result = (struct list_t*) malloc(sizeof(struct list_t));
	result->head = cabeca;
	return result;
}

void list_destroy(struct list_t *list){
	free(list);
}

/* Fun��o que adiciona no final da lista (tail) a entry passada como
 * argumento.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_add(struct list_t *list, struct entry_t *entry){
		return list_add_node(list->head,entry);
	
}
/*
 * Funcao que percorre uma estrutura de nodes, quando encontrar um
 * node a  NULL insere neste valor uma nova struct node_t com o 
 * atributo da entry_t inicializado
 */
int list_add_node(struct node_t *noactual,struct entry_t *entryAPor){
	if (noactual == NULL){
		noactual= (node_t*) malloc(sizeof(struct node_t));
		noactual->elem=entryAPor;
		return 0;
	}else{
		list_add_node(noactual->next,entryAPor);
		
	}
	
}
