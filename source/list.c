/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list-private.h"
#include "entry.h"
#include <string.h>

struct list_t *list_create(){
	struct list_t *newList = (struct list_t*)malloc(sizeof(struct list_t));
	if(newList == NULL){
		return NULL;
	}
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}

void list_destroy(struct list_t *list){
	struct node_t *noCorr;
	struct node_t *nextNode = list->head;
	while(nextNode != NULL){
		noCorr = nextNode;
		if(noCorr->data != NULL){
			entry_destroy(noCorr->data);
		}
		nextNode = noCorr->next;
		free(noCorr);
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
	if (noCorr == NULL || key == NULL){
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

char **list_get_keys(struct list_t *list){
	char **listKeys = (char**) malloc(((list->size)+1)*(sizeof(char*)));
	struct node_t *noCorr = list->head;
	int i = 0;

	while(noCorr != NULL){
		listKeys[i] = (char *) malloc(sizeof(noCorr->data->key));
		strcpy(listKeys[i], noCorr->data->key);
		noCorr = noCorr->next;
		i++;
	}
	listKeys[i] = NULL;
	return listKeys;
}

void list_free_keys(char **keys){
	int i = 0;
	while(keys[i] != NULL){
		free(keys[i]);
		i++;
	}
	//liberta a ultima posicao NULL
	free(keys[i]);
	free(keys);
}

void list_print(struct list_t* list){
	struct node_t *noCorr = list->head;
	if(noCorr == NULL){
		printf("A lista estah vazia.\n");
		return;
	}
	while(noCorr != NULL){
		if(noCorr->next != NULL){
			printf("| Chave: %s | Valor: %s |   ===>  ",noCorr->data->key, (char*) noCorr->data->value->data);
			noCorr = noCorr->next;
		}else{
			printf("| Chave: %s | Valor: %s |",noCorr->data->key, (char*) noCorr->data->value->data);
			break;
		}
	}
	printf("\n");
}