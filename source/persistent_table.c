/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "persistent_table.h"
#include "persistent_table-private.h"
#include "persistence_manager.h"

extern int KEEP_LOGS;

int persist_message(struct ptable_t *ptable, struct message_t *op) {
    int result = -1;
    while (1 == 1) {
        if(pmanager_log(ptable->pmanager, op) == -1) {
            if (pmanager_store_table(ptable->pmanager, ptable->table) == -1) {
                result = -1;
                break;
            }
            result = pmanager_rotate_log(ptable->pmanager);
            if(result == -1) {
                break;
            }
        } else {
            result = 0;
            break;
        }
    }
    return result;
}

/* Cria uma tabela persistente, passando como parâmetro a tabela
 * a ser mantida em memória e o gestor de persistência a ser usado para 
 * manter logs e checkpoints. 
 * Retorna a tabela persistente criada ou NULL em caso de erro.
 */
struct ptable_t *ptable_create(struct table_t *table, struct pmanager_t *pmanager) {
    
    if (table == NULL) return NULL;
    if (pmanager == NULL) return NULL;
    
    struct ptable_t *result;
    if ((result = (struct ptable_t*) malloc(sizeof(struct ptable_t))) == NULL) {
        return NULL;
    }

    result->pmanager = pmanager;
    result->table = table;

    return result;
}

/* Liberta toda a memória ocupada pela tabela persistente e apaga
 * todos os ficheiros utilizados pela tabela.
 */
void ptable_destroy(struct ptable_t *ptable) {
    if(KEEP_LOGS > 0) {
        if(pmanager_destroy(ptable->pmanager) == -1) {
            printf("\n\nThere was an error while destroying pmanager!\n\n");
        }
    } else {
        if (pmanager_destroy_clear(ptable->pmanager) == -1) {
            printf("\n\nThere was an error while destroying pmanager!\n\n");
        }
    }
    table_destroy(ptable->table);
    free(ptable);
}

/* Função para adicionar um elemento na tabela.
 * A função vai *COPIAR* a key (string) e os dados para um novo espaço
 * de memória alocado por malloc().
 * Se a key já existir na tabela, a função tem de substituir a entrada
 * existente pela nova, fazendo a necessária gestão da memória para
 * armazenar os novos dados.
 * Devolve 0 (ok), em caso de adição ou substituição, ou -1 (erro), em
 * caso de erro.
 */
int ptable_put(struct ptable_t *ptable, char *key, struct data_t *data) {
    //A duplicacao de data e key eram para ser feitas em table.c na 1a fase
    //assim n eh necessario fazer duplicacao aqui
    int key_len = 0;
    char* key_cpy = NULL;
    struct data_t *data_cpy = NULL;
    struct message_t *op = NULL;

    if ( (ptable == NULL) || (key == NULL) || (data == NULL) ) {
        printf("\n\nNO NULL PARAMS!\n\n");
        return -1;
    }

    // puts data at the table
    if(table_put(ptable->table, key, data) == -1) {
        printf("\n\nPut at ptable_put failed\n\n");
        free(key);
        data_destroy(data);
        return -1;
    }

    // create tmp message to persist the operation
    op = (struct message_t*) malloc(sizeof(struct message_t));
    op->opcode = OP_PUT;
    op->c_type = CT_ENTRY;
    op->content.entry = entry_create(strdup(key), data_dup(data));
    // persist
    if (persist_message(ptable, op) == -1) {
        free(op);
        return -1;
    }
    // free message struct
    free_message(op);
    return 0;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função aloca memória para armazenar uma *COPIA* dos dados da tabela
 * e retorna o endereço desta memória. O programa que chama esta função
 * é responsável por libertar esta memória.
 * Devolve NULL em caso de erro. 
 */
struct data_t *ptable_get(struct ptable_t *ptable, char *key) {
    struct data_t *result = NULL;
    
    if (ptable == NULL || key == NULL) {
        printf("\n\nNO NULL PARAMS!\n\n");
        return NULL;
    }

    result = table_get(ptable->table, key);
    return result;
}

/* Função para remover um elemento da tabela, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação ptable_put.
 * Retorna 0 (ok) ou -1 (key not found or error).
 */
int ptable_del(struct ptable_t *ptable, char *key) {
    
    struct message_t *op = NULL;

    if (ptable == NULL || key == NULL) {
        printf("\n\nNO NULL PARAMS!\n\n");
        return -1;
    }

    if (table_del(ptable->table, key) < 0) {
        // nothing to delete
        return -1;
    }


    // create tmp message to persist the operation
    op = (struct message_t*) malloc(sizeof(struct message_t));
    op->opcode = OP_DEL;
    op->c_type = CT_KEY;
    op->content.key = key;
    // persist
    if (persist_message(ptable, op) == -1) {
        free(op);
        return -1;
    }
    // free message struct, content is destroyed by table_skel
    free(op);

    return 0;
}

/* Devolve o número de elementos na tabela ou -1 em caso de erro.
 */
int ptable_size(struct ptable_t *ptable) {
    if (ptable == NULL) {
        printf("\n\nNO NULL PARAM!\n\n");
        return -1;
    }
    int result = table_size(ptable->table);
    return result;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **ptable_get_keys(struct ptable_t *ptable) {
    if (ptable == NULL) {
        printf("\n\nNO NULL PARAM!\n\n");
        return NULL;
    }
    char **result = table_get_keys(ptable->table);
    return result;
}

/* Função que liberta toda a memória alocada por ptable_get_keys().
 */
void ptable_free_keys(char **keys) {
    int i = 0;
    while(keys[i] != NULL){
            free(keys[i]);
            i++;
    }
    free(keys);
}
