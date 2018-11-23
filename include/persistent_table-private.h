#include "table-private.h"
#include "persistence_manager-private.h"

struct ptable_t {
    struct table_t *table;
    struct pmanager_t *pmanager;
};
