#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list-private.h"
#include "entry.h"

struct list_t *list_create(){
	struct list_t *newList = (struct list_t*)malloc(sizeof(struct list_t));
	if(newList == NULL){
		return NULL;
	}
	newList->head = NULL;
	newList->size = 0;
	return newList;
}

void list_destroy(struct list_t *list){
	struct node_t *noCorr = list->head;
	while(noCorr != NULL){
		free(noCorr);
		noCorr = noCorr->next;
	}
	free(list);
}

int list_add(struct list_t *list, struct entry_t *entry){
	//Caso a lista esteja vazia
	if(list->head == NULL){
		struct node_t *newNode = (struct node_t*)malloc(sizeof(struct node_t));
		if(newNode == NULL){
			return -1;
		}else{
			newNode->data = entry;
			newNode->next = NULL;
			list->head = newNode;
			list->size = (list->size)+1;
			return 0;
		}
	}
	
	struct node_t *noCorr = list->head;
	while(noCorr->next != NULL){
		noCorr = noCorr->next;
	}
	/*Criacao de novo no*/
	struct node_t *nextNode = (struct node_t*)malloc(sizeof(struct node_t));
	if(nextNode == NULL){
		return -1;
	}else{
		nextNode->data = entry;
		nextNode->next = NULL;
		/*Concatenacao do novo no ao ultimo elemento da lista corrente*/
		list->size = (list->size)+1;
		noCorr->next = nextNode;
		return 0;
	}
}

int list_remove(struct list_t *list, char *key){
	struct node_t *noCorr = list->head, *noAnt;
	
	if(noCorr != NULL && (strcmp(noCorr->data->key,key) == 0)){
		list->head = noCorr->next;
		/*A funcao entry_destroy trata da libertação de memória das estruturas em niveis inferiores*/
		entry_destroy(noCorr->data);
		free(noCorr);
		list->size = (list->size)-1;
		return 0;
	}
	
	while(noCorr != NULL && strcmp(noCorr->data->key,key) != 0){
		noAnt = noCorr;
		noCorr = noCorr->next;
	}
	
	/*Caso nao exista um entry na lista com a dada chave*/
	if(noCorr == NULL){
		return -1;
	}
	
	noAnt->next = noCorr->next;
	entry_destroy(noCorr->data);
	list->size = (list->size)-1;
	free(noCorr);
	return 0;
}

struct entry_t *list_get(struct list_t *list, char *key){
	struct node_t *noCorr = list->head;
	if (noCorr == NULL){
		return NULL;
	}
	while(noCorr != NULL){
		if(strcmp(noCorr->data->key,key) == 0){
			return noCorr->data;
		}
		noCorr = noCorr->next;
	}
	return NULL;
}

int list_size(struct list_t *list){
	return list->size;
}


void list_print(struct list_t* list){
	struct node_t *noCorr = list->head;
	if(noCorr == NULL){
		return;
	}
	while(noCorr != NULL){
		printf("Valor do noh: %d    |   Chave do noh: %s\n",noCorr->data->value->data, noCorr->data->key);
		noCorr = noCorr->next;
	}
}
