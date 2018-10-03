#include <stdio.h>
#include <stdlib.h>
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

void entry_initialize(struct entry_t *entry) {
	entry->key = NULL;
	entry->value = NULL;
}

void entry_destroy(struct entry_t *entry){
	
	if(entry->value != NULL){
		data_destroy(entry->value);
	}
	if(entry->key != NULL) {
		//TEMPORARY=============================================
		//free(entry->key);
	}
	
	free(entry);
}

struct entry_t *entry_dup(struct entry_t *entry){
	struct entry_t *dupEntry = (struct entry_t *) malloc(sizeof(entry));
	dupEntry->key = strcpy(dupEntry->key, entry->key);
	dupEntry->value = data_dup(entry->value);

	return dupEntry;
}
