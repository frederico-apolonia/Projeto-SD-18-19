#include <stdio.h>
#include <stdlib.h>
#include "data.h"

struct data_t *data_create(int size){
	struct data_t *newData = (struct data_t *) malloc(sizeof(struct data_t));
	
	if(newData == NULL) {
		return NULL; // if malloc failed
	}

	newData->datasize = size;
	newData->data = malloc(size);
	if(newData->data == NULL) {
		// if malloc failed
		free(newData);
		return NULL; 
	}
	return newData;
}

struct data_t *data_create2(int size, void *data){
	struct data_t *newData = data_create(size);

	if(newData == NULL) {
		free(newData);
		return NULL;
	}
	memcpy(newData->datasize, data, size);
	return newData;
}

void data_destroy (struct data_t *data){
	//TEMPORARY==================================================================
	//free(data->data);
	free(data);
}

struct data_t *data_dup(struct data_t *data){
	return data == NULL ? NULL : data_create2(data->datasize, data->data);
}
