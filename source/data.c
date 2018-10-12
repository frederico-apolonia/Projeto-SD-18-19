#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

struct data_t *data_create(int size){
	if(size <= 0) {
		return NULL;
	}
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
	if(newData == NULL || data == NULL) {
		free(newData);
		return NULL;
	}
	free(newData -> data);
	newData -> data = data;
	/*
	"Não. Para duplicar uma estrutura data usam a fução data_dup(). 
	A função data_create2() apenas aloca memória para a estrutura, 
	mas não para os dados apontados pelo campo data. 
	Este campo fica com o ponteiro passado como parâmetro 
	(e portanto esse ponteiro tem de apontar para uma zona de memória 
	já alocada e com os dados) ."
	
	memcpy(newData->data, data, size);*/
	return newData;
}

void data_destroy (struct data_t *data){
	if(data == NULL){
		return;
	}
	
	if(data->data != NULL){
		free(data->data);
	}
	free(data);
}

struct data_t *data_dup(struct data_t *data){
	if(data == NULL) {
		return NULL;
	}

	struct data_t *data_copy;
	if ((data_copy = data_create(data->datasize)) != NULL ) {
		// memoria bem alocada
		if (data->data == NULL) {
			// se o data for nulo, nao existe nada para copiar
			data_destroy(data_copy);
			return NULL;
		}
		memcpy(data_copy->data, data->data, data->datasize);
		return data_copy;
	}
	
	return NULL;
}
