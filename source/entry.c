#include <stdio.h>
#include <stdlib.h>
#include "entry.h"

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){	
	struct entry_t *result = (struct entry_t*) malloc (sizeof(struct entry_t));
	result->key = key;
	result->value = data;
	return result;	
}

/* Função que inicializa os elementos de uma entrada na tabela com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){
	entry->key= NULL;
	entry->value= NULL;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
	free(entry);
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){
	return entry_create(entry->key,entry->value);
}
