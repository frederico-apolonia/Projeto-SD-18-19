#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entry.h"
#include "data.h"

struct entry_t *entry_create(char *key, struct data_t *data){
	struct entry_t *newEntry = (struct entry_t *) malloc(sizeof(struct entry_t));

	if(newEntry == NULL) {
		return NULL;
	}

	newEntry->key = key;
	newEntry->value = data;
	return newEntry;
}

void entry_destroy(struct entry_t *entry){
	
	if(entry == NULL) {
		return;
	}

	if(entry->value != NULL){
		data_destroy(entry->value);
	}
	if(entry->key != NULL) {
		free(entry->key);
	}
	
	free(entry);
}

struct entry_t *entry_dup(struct entry_t *entry){
	if(entry == NULL){
		return NULL;
	}
	struct entry_t *dupEntry = (struct entry_t *) malloc(sizeof(struct entry_t));
	if(dupEntry == NULL){
		return NULL;
	}

	int key_size = strlen(entry->key)+1; // \0
	dupEntry->key = (char*) malloc(key_size);
	if(dupEntry->key == NULL) {
		free(dupEntry);
		return NULL;
	}
	strcpy(dupEntry->key, entry->key);
	dupEntry->value = data_dup(entry->value);
	if(dupEntry->value == NULL) {
		free(dupEntry->key);
		free(dupEntry);
		return NULL;
	}
	return dupEntry;
}
