#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size){
	struct data_t *result = (struct data_t*) malloc(size);
	return result;	
}

/* Função que cria um novo elemento de dados data_t, de acordo com o parametro data
 * e reserva a memória necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create2(int size, void *data){
	struct data_t *result = (struct data_t*)malloc(size);
	result->data = data;
	return result;	
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){
	free(data);
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
	struct data_t *dupData = malloc(data->datasize);
	dupData->data = data->data;
	return dupData;
}


