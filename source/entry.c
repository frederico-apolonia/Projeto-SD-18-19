#include <stdio.h>
#include <stdlib.h>
#include "entry.h"

struct entry_t *entry_create(char *key, struct data_t *data){	
	struct entry_t *result = (struct entry_t*) malloc (sizeof(struct entry_t));
	result->key = key;
	result->value = data;
	return result;	
}

void entry_initialize(struct entry_t *entry){
	entry->key = NULL;
	entry->value = NULL;
}

void entry_destroy(struct entry_t *entry){
	if(entry->key != NULL) {
		free(entry->key);
	}
	if(entry->value != NULL) {
		free(entry->value);
	}
	free(entry);
}

struct entry_t *entry_dup(struct entry_t *entry){
	return entry_create(entry->key,entry->value);
}
