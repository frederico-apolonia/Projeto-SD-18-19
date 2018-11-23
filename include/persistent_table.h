#ifndef _PERSISTENT_TABLE_H
#define _PERSISTENT_TABLE_H

#include "data.h"
#include "table.h"
#include "persistence_manager.h"

struct ptable_t; /* A definir em persistent_table-private.h */

/* Cria uma tabela persistente, passando como parâmetro a tabela
 * a ser mantida em memória e o gestor de persistência a ser usado para 
 * manter logs e checkpoints. 
 * Retorna a tabela persistente criada ou NULL em caso de erro.
 */
struct ptable_t *ptable_create(struct table_t *table, 
                             struct pmanager_t *pmanager);

/* Liberta toda a memória ocupada pela tabela persistente e apaga
 * todos os ficheiros utilizados pela tabela.
 */
void ptable_destroy(struct ptable_t *ptable);

/* Função para adicionar um elemento na tabela.
 * A função vai *COPIAR* a key (string) e os dados para um novo espa�o
 * de memória alocado por malloc().
 * Se a key já existir na tabela, a função tem de substituir a entrada
 * existente pela nova, fazendo a necessária gestão da memória para
 * armazenar os novos dados.
 * Devolve 0 (ok), em caso de adição ou substituição, ou -1 (erro), em
 * caso de erro.
 */
int ptable_put(struct ptable_t *ptable, char *key, struct data_t *data);

/* Função para obter da tabela o valor associado à chave key.
 * A função aloca memória para armazenar uma *COPIA* dos dados da tabela
 * e retorna o endereço desta memória. O programa que chama esta função
 * é responsável por libertar esta memória.
 * Devolve NULL em caso de erro. 
 */
struct data_t *ptable_get(struct ptable_t *ptable, char *key);

/* Função para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação ptable_put.
 * Retorna 0 (ok) ou -1 (key not found or error).
 */
int ptable_del(struct ptable_t *ptable, char *key);

/* Devolve o número de elementos na tabela ou -1 em caso de erro.
 */
int ptable_size(struct ptable_t *ptable);

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **ptable_get_keys(struct ptable_t *ptable);

/* Função que liberta toda a memória alocada por ptable_get_keys().
 */
void ptable_free_keys(char **keys);

#endif
