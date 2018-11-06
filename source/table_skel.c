#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"
#include "message-private.h"
#include "table_skel.h"
#include "table.h"
#include "table-private.h"

extern struct table_t * server_table;

int table_skel_init(int n_lists) {
    server_table = table_create(n_lists);
    return (server_table != NULL) ? 0:1;
}

int table_skel_destroy() {
    table_destroy(server_table);
}

int build_error_message(struct message_t *msg) {
    msg->opcode = OP_ERROR;
    msg->c_type = CT_NONE;
}

int invoke(struct message_t *msg) {
    int result = 0;
    struct data_t* data_aux;

    switch (msg->opcode)
    {
        case OP_SIZE:
            msg->opcode += 1;
            msg->c_type = CT_RESULT;
            msg->content.result = table_size(server_table);
            result = 0;
            break;

        case OP_DEL:
            if (msg->c_type != CT_KEY) {
                result = -1;
                break;
            }
            msg->opcode += 1;
            msg->c_type = CT_NONE;
            result = table_del(server_table, msg->content.key);
            free(msg->content.key);
            break;
    
        case OP_GET:
            if (msg->c_type != CT_KEY) {
                result = -1;
                break;
            }
            msg->opcode += 1;
            msg->c_type = CT_VALUE;
            data_aux = table_get(server_table, msg->content.key);
            if(data_aux == NULL){
            	data_aux = malloc(sizeof(struct data_t*));
            	data_aux->datasize = 0;
            	data_aux->data = NULL;
			}
            free(msg->content.key);
            msg->content.value = data_aux;
            result = data_aux != NULL ? 0:-1;
            break;

        case OP_PUT:
            if (msg->c_type != CT_ENTRY) {
                result = -1;
                break;
            }
            msg->opcode += 1;
            msg->c_type = CT_NONE;
            result = table_put(server_table, strdup(msg->content.entry->key), data_dup(msg->content.entry->value));
            entry_destroy(msg->content.entry);
            break;

        case OP_GETKEYS:
            msg->opcode += 1;
            msg->c_type = CT_KEYS;
            msg->content.keys = table_get_keys(server_table);
            result = msg->content.keys != NULL ? 0:-1;
            break;

        default:
            result = -1;
            break;
    }
    printf("Table State: \n");
	table_print(server_table);
    return result;
    
}
