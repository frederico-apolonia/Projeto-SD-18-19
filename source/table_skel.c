/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "message.h"
#include "message-private.h"
#include "table_skel.h"
#include "persistent_table.h"
#include "persistent_table-private.h"

extern struct ptable_t *ptable;

int table_skel_init(int n_lists, char *log_filename, int log_size) {
    struct table_t *table;
    struct pmanager_t *pmanager;

    /* init table */
    if ((table = table_create(n_lists)) == NULL) {
        printf("Table init failed\n");
        return -1;
    }
    /* init pmanager */
	if((pmanager = pmanager_create(log_filename, log_size)) == NULL) {
		printf("Persistence manager creation failed\n");
		table_destroy(table);
		return -1;
	}
    /* init ptable */
	if((ptable = ptable_create(table, pmanager)) == NULL) {
		printf("Ptable creation failed\n");
		table_destroy(table);
		pmanager_destroy_clear(pmanager);
		return -1;
	}

	if(pmanager_have_data(pmanager) == 1) {
		if (pmanager_fill_state(pmanager, table) == -1) {
			printf("Table recover to memory failed\n");
			return -1;
		}
	}

    return 0;
}

int table_skel_destroy() {
    table_destroy(ptable->table);
}

int build_error_message(struct message_t *msg) {
    msg->opcode = OP_ERROR;
    msg->c_type = CT_NONE;
}

int invoke(struct message_t *msg) {
    int result = 0;
    char *msg_key = NULL;
    struct data_t* data_aux = NULL;
    struct data_t *msg_data = NULL;

    switch (msg->opcode)
    {
        case OP_SIZE:
            msg->opcode += 1;
            msg->c_type = CT_RESULT;
            msg->content.result = ptable_size(ptable);
            result = 0;
            break;

        case OP_DEL:
            if (msg->c_type != CT_KEY) {
                result = -1;
                break;
            }
            msg->opcode += 1;
            msg->c_type = CT_NONE;

            msg_key = msg->content.key;

            result = ptable_del(ptable, msg_key);
            free(msg->content.key);
            break;
    
        case OP_GET:
            if (msg->c_type != CT_KEY) {
                result = -1;
                break;
            }
            msg->opcode += 1;
            msg->c_type = CT_VALUE;

            msg_key = msg->content.key;
            
            data_aux = ptable_get(ptable, msg_key);
            if(data_aux == NULL){
            	data_aux = malloc(sizeof(struct data_t*));
            	data_aux->datasize = 0;
            	data_aux->data = NULL;
			}
            free(msg->content.key);
            msg->content.value = data_aux;
            result = data_aux->data != NULL ? 0:-1;
            break;

        case OP_PUT:
            if (msg->c_type != CT_ENTRY) {
                result = -1;
                break;
            }
            msg_key = msg->content.entry->key;
            msg_data = msg->content.entry->value;

            if (ptable_put(ptable, msg_key, msg_data) != -1) {
                msg->opcode += 1;
                msg->c_type = CT_NONE;
                result = 0;
            } else {
                printf("Error while processing put...\n");
                result = -1;
            }
            // destroy entry to avoid mem leaks!
            entry_destroy(msg->content.entry);
            break;

        case OP_GETKEYS:
            msg->opcode += 1;
            msg->c_type = CT_KEYS;
            msg->content.keys = ptable_get_keys(ptable);
            result = msg->content.keys != NULL ? 0:-1;
            break;

        default:
            result = -1;
            break;
    }
    printf("Table State: \n");
	table_print(ptable->table);
    return result;
    
}
