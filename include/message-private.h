/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include "message.h"

/* Altera os campos OP_CODE e CT_CODE de uma mensagem para
 * OP_ERROR e CT_NONE, tornando a mensagem numa mensagem de erro.
 */
int build_error_message(struct message_t *msg);

/* Imprime uma mensagem (tirado do test_message) */
void print_message(struct message_t* msg);