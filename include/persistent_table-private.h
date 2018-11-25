/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include "table-private.h"
#include "persistence_manager-private.h"

struct ptable_t {
    struct table_t *table;
    struct pmanager_t *pmanager;
};
