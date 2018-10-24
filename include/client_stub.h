#ifndef _REMOTE_TABLE_H
#define _REMOTE_TABLE_H

#include "data.h"
#include "entry.h"

/* Remote table. A definir pelo grupo em client_stub-private.h
*/
struct rtable_t;

/* Função para estabelecer uma associação entre o cliente e uma tabela no 
 * servidor.
 * address_port é uma string no formato <hostname>:<port>.
 * retorna NULL em caso de erro
 */
struct rtables_t *rtables_connect(const char *address_port);

/* Termina a associação entre o cliente e a tabela remota, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtables_disconnect(struct rtables_t *rtables);

/* Funcao para adicionar um elemento na tabela.
 * Se a key ja existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry);

/* Funcao para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key);

/* Funcao para remover um elemento da tabela. Vai libertar 
 * toda a memoria alocada na respectiva operacao rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key);

/* Devolve o número de elementos da tabela.
 */
int rtable_size(struct rtable_t *rtable);

/* Devolve um array de char* com a copia de todas as keys da tabela,
 * colocando um ultimo elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable);

/* Liberta a memoria alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys);

#endif

