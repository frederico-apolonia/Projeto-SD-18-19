/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
	int size;
	int numElems;
	struct list_t **lists;
};

/* Função para criar/inicializar uma nova tabela hash, com n linhas 
 * (módulo da função HASH).
 */
struct table_t *table_create(int n);

/* Função que imprime a tabela */
void table_print(struct table_t *table);

/* Função para calcular o hash da chave inserida */
int hash(char* str, int size);

#endif
