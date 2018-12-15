/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "data.h"
#include "entry.h"
#include "network_client.h"
#include "client_stub.h"
#include "client_stub-private.h"
#include "message-private.h"

/* Percorre keys até NULL para calcular o numero de chaves */
int get_num_keys(char** keys) {
	int i = 0;
	while (keys[i] != NULL) {
		i++;
	}
	return i;
}

/* Funcao que separar uma string em varios tokens separados por
 * seperator. Guarda o numero de tokens em counter
 */
char** tokenizer(char* string, char* seperator, int* counter) {
	*counter = 0;
	char** result = (char**) malloc(sizeof(char*));
	char* rest = string;
	char* tmp = strtok_r(string, seperator, &rest);

	while(1 == 1) {
		result = realloc(result, (sizeof(char*) * (*counter + 1)));
		result[*counter] = tmp;
		tmp = strtok_r(NULL, seperator, &rest);
		if(tmp == NULL) {
			break;
		}
		*counter += 1;
	}
	*counter += 1;

	return result;
}

/* FunÃ§Ã£o para estabelecer uma associaÃ§Ã£o entre o cliente e uma tabela no
 * servidor.
 * address_port Ã© uma string no formato <hostname>:<port>.
 * retorna NULL em caso de erro
 */
struct rtable_t *rtables_connect(char *address_port){
	int sockfd;
	struct rtable_t* rtable;
	int size_adr_port;
	//Divide a string em dois para se obter o hostname em hostname e o port em port_string
	char** address_tokenized = tokenizer(strdup(address_port), ":", &size_adr_port);

	if(size_adr_port != 2) {
		printf("%d, %s\n", size_adr_port, address_port);
		printf("Table-client must be invoked like this:\n");
		printf("./table-client <ip>:<port>\n");
		exit(EXIT_FAILURE);
	}

	if ((rtable = malloc(sizeof(struct rtable_t))) == NULL) {
		printf("DEBUG FAILED TO MALLOC\n");
		exit(EXIT_FAILURE);
	}
	rtable->ip = address_tokenized[0];
	rtable->port = address_tokenized[1];

	if((network_connect(rtable)) == -1 ) {
		free(rtable);
		printf("DEBUG: Failed to connect\n");
		return NULL;
	}

	return rtable;
}

/* Termina a associaÃ§Ã£o entre o cliente e a tabela remota, fechando a
 * ligaÃ§Ã£o com o servidor e libertando toda a memÃ³ria local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtables_disconnect(struct rtable_t *table){
	network_close(table);
	free(table);
	return 0;
}

/* Funcao para adicionar um elemento na tabela.
 * Se a key ja existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adiÃ§Ã£o/substituiÃ§Ã£o) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry){
	
	if(rtable == NULL) {
		printf("DEBUG: rtable de entrada do put a NULL\n");
		return -1;
	}

	if(entry == NULL) {
		printf("DEBUG: entry de entrada do put a NULL\n");
		return -1;
	}

	struct message_t *msg_pedido, *msg_resposta = NULL;
	//Usar o codigo 40
	msg_pedido = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg_pedido == NULL){
		printf("DEBUG: Malloc msg_pedido falhou\n");
		return -1;
	}
	msg_pedido->opcode = OP_PUT;
	msg_pedido->c_type = CT_ENTRY;
	msg_pedido->content.entry = entry;
	
	msg_resposta = network_send_receive(rtable,msg_pedido);
	free_message(msg_pedido);
	if (msg_resposta == NULL){
		free_message(msg_resposta);
		return -1;
	}
	if((msg_resposta->opcode) == OP_ERROR) {
		free_message(msg_resposta);
		return -1;
	}

	free_message(msg_resposta);
	return 0;
}

/* Funcao para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key){
	if(rtable == NULL || key == NULL){
		return NULL;
	}

	struct message_t *msg_pedido, *msg_resposta = NULL;
	//Usar o codigo 30
	msg_pedido = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg_pedido == NULL){
		printf("Falhou alloc msg pedido\n");
		return NULL;
	}
	msg_pedido->opcode = OP_GET;
	msg_pedido->c_type = CT_KEY;
	msg_pedido->content.key = strdup(key);

	msg_resposta = network_send_receive(rtable,msg_pedido);
	free_message(msg_pedido);
	
	// resposta
	if (msg_resposta == NULL) {
		// printf("ERROR: Server error.\n");
		return NULL;
	} 
	if (msg_resposta->opcode == OP_ERROR){
		free_message(msg_resposta);
		struct data_t *result = data_create(1);
		return result;
	}
	// chave nao existe
	if (msg_resposta->content.value->datasize == 0) {
		// printf("ERROR: Key not found!\n");
		free_message(msg_resposta);
		struct data_t *result = data_create(1);
		return result;
	}

	struct data_t *result = data_dup(msg_resposta->content.value);
	free_message(msg_resposta);
	return result;
}

/* Funcao para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respectiva operacao rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key){
	if(rtable == NULL || key == NULL){
		return -1;
	}
	struct message_t *msg_pedido, *msg_resposta = NULL;
	//Usar o codigo 20
	msg_pedido = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg_pedido == NULL){
		return -1;
	}
	msg_pedido->opcode = OP_DEL;
	msg_pedido->c_type = CT_KEY;

	if((msg_pedido->content.key = (char*) malloc(strlen(key) + 1)) == NULL) {
		free(msg_pedido);
		return -1;
	}

	strncpy(msg_pedido->content.key, key, strlen(key)+1);

	msg_resposta = network_send_receive(rtable,msg_pedido);
	free_message(msg_pedido);
	if( msg_resposta == NULL || (msg_resposta->opcode) == OP_ERROR){
		free_message(msg_resposta);
		return -1;
	}
	free(msg_resposta);
	return 0;


}

/* Devolve o nÃºmero de elementos da tabela.
 */
int rtable_size(struct rtable_t *rtable){
	if(rtable == NULL){
		return -1;
	}
	struct message_t *msg_pedido, *msg_resposta = NULL;
	//Usar o codigo 10
	msg_pedido = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg_pedido == NULL){
		return -1;
	}
	msg_pedido->opcode = OP_SIZE;
	msg_pedido->c_type = CT_NONE;
	printf("MSG SIZE CONSTRUIDA\n");

	msg_resposta = network_send_receive(rtable,msg_pedido);
	free_message(msg_pedido);
	if (msg_resposta == NULL || (msg_resposta->opcode) == OP_ERROR){
		return -1;
	}
	int result = msg_resposta->content.result;
	free_message(msg_resposta);
	return result;
}

/* Devolve um array de char* com a copia de todas as keys da tabela,
 * colocando um ultimo elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable){
	if(rtable == NULL){
		return NULL;
	}
	struct message_t *msg_pedido, *msg_resposta = NULL;
	int num_keys;
	char** result_keys;
	//Usar o codigo 50
	msg_pedido = (struct message_t*) malloc(sizeof(struct message_t));
	if (msg_pedido == NULL){
		return NULL;
	}
	msg_pedido->opcode = OP_GETKEYS;
	msg_pedido->c_type = CT_NONE;
	msg_resposta = network_send_receive(rtable,msg_pedido);
	free_message(msg_pedido);
	if (msg_resposta == NULL || (msg_resposta->opcode) == OP_ERROR || msg_resposta->content.keys == NULL){
		free_message(msg_pedido);
		return NULL;
	}

	num_keys = get_num_keys(msg_resposta->content.keys);
	if ((result_keys = (char**) malloc((num_keys+1) * sizeof(char*))) == NULL) {
		free_message(msg_resposta);
		return NULL;
	}

	for(int i = 0; i < num_keys; i++) {
		result_keys[i] = (char*) malloc(strlen(msg_resposta->content.keys[i]) + 1);
		strcpy(result_keys[i], msg_resposta->content.keys[i]);
	}
	result_keys[num_keys] = NULL;

	return result_keys;
}

/* Liberta a memoria alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys){
	int i = 0;
	if(keys == NULL){
		return;
	}

	while(keys[i] != NULL){
		free(keys[i]);
		i++;
	}
	//liberta a ultima posicao NULL
	free(keys[i]);
	free(keys);

}