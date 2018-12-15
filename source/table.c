/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "table-private.h"
#include "list.h"
#include "list-private.h"
#include "entry.h"

struct table_t *table_create(int n){
	struct table_t *newTable = (struct table_t*) malloc(sizeof(struct table_t));
	if(newTable == NULL){
		return NULL;
	}
	if(n <= 0){
		free(newTable);
		return NULL;
	}
	newTable->size = n;
	newTable->numElems = 0;
	newTable->lists = (struct list_t**) malloc(sizeof(struct list_t**)*n);
	if(newTable->lists == NULL){
		free(newTable);
		return NULL;
	}
	int i, j;
	for(i = 0; i<n; i++){
		newTable->lists[i] = list_create();
		if(newTable->lists[i] == NULL){
			/* Se alguma das criacoes falhar, ir as listas que ja foram criadas
			 * e liberta-las.
			 */
			for(j = 0; j<i; j++){
				list_destroy(newTable->lists[j]);
			}
			return NULL;
		}
	}
	return newTable;
}

void table_destroy(struct table_t *table){
	int i;
	// destruir as listas
	for(i = 0; i < (table->size); i++){
		list_destroy(table->lists[i]);
	}
	// libertar estrutura da tabela
	free(table->lists);
	free(table);
}

int table_put(struct table_t *table, char *key, struct data_t *value){
	int hashIndex = hash(key,table->size);
	struct entry_t *checkEntry = list_get(table->lists[hashIndex],key);
	struct entry_t *newEntry = entry_create(key,value);
	if(newEntry == NULL){
		return -1;
	}
	//Verificacao da existencia de uma mesma entrada
	if(checkEntry != NULL){
		//Remocao e adicao
		if(list_remove(table->lists[hashIndex],key) == -1){
			free(checkEntry);
			free(newEntry);
			return -1;
		}
		
		if(list_add(table->lists[hashIndex],newEntry) == -1){
			free(checkEntry);
			free(newEntry);
			return -1;
		}

		return 0;

	}else{
		//Adicao
		if(list_add(table->lists[hashIndex],newEntry) == -1){
			free(newEntry);
			return -1;
		}
		//adiciona-se 1 ao total de elementos
		table->numElems += 1;
		return 0;
	}
}

struct data_t *table_get(struct table_t *table, char *key){
	if(table == NULL) {
		printf("TABLE IS NULL!\n");
		return NULL;
	}
	int hashIndex = hash(key,table->size);
	struct entry_t *entry = list_get(table->lists[hashIndex],key);
	if(entry == NULL || entry->value == NULL){
		// printf("ENTRY ERRO\n");
		return NULL;
	}

	struct data_t *newData = data_dup(entry->value);
	if(newData == NULL){
		printf("NEW DATA ERRO!\n");
		return NULL;
	}else{
		return newData;
	}
}

int table_del(struct table_t *table, char *key) {
	if(table == NULL || key == NULL) {
		return -1;
	}
	// calcula posicao da key
	int hash_index = hash(key, table->size);
	if(list_remove(table->lists[hash_index], key) == -1) {
		return -1;
	} 
	table->numElems -= 1;
	return 0;
}

int table_size(struct table_t *table){
	return table->numElems;
}

char **table_get_keys(struct table_t *table){
	char **tableKeys = (char **) malloc((sizeof(char *)+1)*table_size(table));
	char **currKeys; // chaves da lista a ser 'visitada'
	//i -> contador das listas da table
	//j -> contador do vetor de keys de uma lista
	//k -> contador para o array final contendo todas as keys de todas as listas
	int i,j,k=0;
	for(i=0; i<table->size; i++){
		currKeys = list_get_keys(table->lists[i]);
		for(j=0; j<table->lists[i]->size; j++){
			tableKeys[k] = malloc(sizeof(char)*strlen(currKeys[j]));
			strcpy(tableKeys[k], currKeys[j]);
			k++;
		}
		list_free_keys(currKeys);
	}
	tableKeys[k] = NULL;
	return tableKeys;
}

void table_free_keys(char **keys){
	int i = 0;

	if(keys == NULL){
		return;
	}

	while(keys[i] != NULL){
		free(keys[i]);
		i++;
	}
	//liberta a ultima posicao NULL
	free(keys[i]);
	free(keys);
}

void table_print(struct table_t *table){
	int i;
	for(i = 0; i<table->size; i++){
		printf("Lista %d ====> ", i);
		list_print(table->lists[i]);
	}
}

int hash(char* str, int size){
	int i,val = 0;
	for(i = 0; i < strlen(str); i++){
		val+= (int) str[i];
	}
	return val % size;
}
