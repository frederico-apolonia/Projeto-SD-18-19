#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "list-private.h"
/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
 * grupo no ficheiro list-private.h).
 * Em caso de erro, retorna NULL.
 */
struct list_t *list_create(){
	struct node_t *cabeca = (node_t*) malloc(sizeof(node_t));
	struct list_t *result = (list_t*) malloc(sizeof(list_t));
	result->head = cabeca;
	return result;
}

/* Função que elimina uma lista, libertando *toda* a memoria utilizada
 * pela lista.
 */
void list_destroy(struct list_t *list){
	free(list);
}

/* Função que adiciona no final da lista (tail) a entry passada como
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
