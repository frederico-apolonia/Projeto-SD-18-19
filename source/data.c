#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

struct data_t *data_create(int size){
	struct data_t *result = (struct data_t*) malloc(sizeof(struct data_t));
	result->datasize = size;
	// aloca o espaço especificado pelo param size
	result->data = (void *) malloc(size);
	return result;	
}

struct data_t *data_create2(int size, void *data){
	struct data_t *result = (struct data_t*) malloc(sizeof(struct data_t));
	result->datasize = size;
	result->data = data;
	return result;	
}

void data_destroy(struct data_t *data){
	if(data->data != NULL) {
		free(data);
	}
	free(data);
}

struct data_t *data_dup(struct data_t *data){
	struct data_t *dupData = data_create(data->datasize);
	// TODO dup data
	return dupData;
}
