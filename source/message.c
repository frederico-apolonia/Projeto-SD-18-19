/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "message.h"
#include "base64.h"
#include "message-private.h"

/* Le uma chave do buffer */
char* get_key_from_buffer(char *msg_buf, int *buf_count) {
	short keysize;
	memcpy(&keysize, msg_buf+*buf_count, sizeof(keysize));
	*buf_count += sizeof(keysize);

	char *key = malloc(keysize + 1);
	if(key == NULL){
		return NULL;
	}
	strncpy(key, msg_buf+*buf_count, ntohs(keysize));
	key[ntohs(keysize)] = '\0';

	*buf_count += ntohs(keysize);
	
	return key;
}

/*Funcao que coloca a Key de uma mensagem no buffer*/
int buffer_key(struct message_t *msg, char **msg_buff){

	short keysize = strlen(msg->content.key);
	int size = 2+2+2+keysize;
	*msg_buff = (char *)malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	keysize = htons(keysize);
	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	memcpy(*msg_buff+4, &keysize, 2);
	memcpy(*msg_buff+6, msg->content.key, ntohs(keysize));
	return size;
}

/*Funcao que coloca a Value de uma mensagem no buffer*/
int buffer_value(struct message_t *msg, char **msg_buff) {
	int enc_size = 0, buf_size = 8; // short + short + int
	int buf_count = 0;
	int datasize = 0, datasize_htonl = 0;
	short opcode = 0, c_type = 0;
	char *enc_data = NULL;

	if ((*msg_buff = (char *) malloc(buf_size)) == NULL) {
		return -1;
	}
	// copy opcode to buffer
	opcode = htons(msg->opcode);
	memcpy(*msg_buff, &opcode, sizeof(opcode));
	buf_count += sizeof(opcode);
	// copy c_type to buffer
	c_type = htons(msg->c_type);
	memcpy(*msg_buff + buf_count, &c_type, sizeof(c_type));
	buf_count += sizeof(c_type);

	datasize = msg->content.value->datasize;
	if (datasize == 0 || msg->content.value->data == NULL) {
		// means key wasnt found
		memcpy(*msg_buff + buf_count, &datasize, sizeof(datasize));
		buf_count += sizeof(datasize);
		return buf_size;
	}

	enc_size = base64_encode_alloc(msg->content.value->data, datasize, &enc_data);
	buf_size += enc_size;
	if((*msg_buff = realloc(*msg_buff, buf_size)) == NULL) {
		return -1;
	}
	// copy datasize to buffer
	datasize_htonl = htonl(enc_size);
	memcpy(*msg_buff + buf_count, &datasize_htonl, sizeof(datasize_htonl));
	buf_count += sizeof(datasize_htonl);
	// copy encoded data to buffer
	strncpy(*msg_buff + buf_count, enc_data, enc_size);
	free(enc_data);

	return buf_size;
}

/*Funcao que coloca a Entry de uma mensagem no buffer*/
int buffer_entry(struct message_t *msg, char **msg_buff) {
	int enc_size = 0;
	int buf_count = 0, buf_size = 6; // // short + short + short
	int datasize = 0, datasize_htonl = 0;
	short keysize = 0;
	short opcode = 0, c_type = 0;
	char *enc_data = NULL;

	if ((*msg_buff = (char*) malloc(buf_size)) == NULL) {
		return -1;
	}
	// copy opcode to buffer
	opcode = htons(msg->opcode);
	memcpy(*msg_buff, &opcode, sizeof(opcode));
	buf_count += sizeof(opcode);
	// copy c_type to buffer
	c_type = htons(msg->c_type);
	memcpy(*msg_buff + buf_count, &c_type, sizeof(c_type));
	buf_count += sizeof(c_type);
	// copy keysize to buffer
	keysize = htons(strlen(msg->content.entry->key));
	memcpy(*msg_buff + buf_count, &keysize, sizeof(keysize));
	buf_count += sizeof(keysize);
	// realloc with keysize
	buf_size += strlen(msg->content.entry->key);
	if ((*msg_buff = realloc(*msg_buff, buf_size)) == NULL) return -1;
	// copy key to buffer 
	strncpy(*msg_buff + buf_count, msg->content.entry->key, strlen(msg->content.entry->key));
	buf_count += strlen(msg->content.entry->key);
	// encode value
	datasize = msg->content.entry->value->datasize;
	enc_size = base64_encode_alloc(msg->content.entry->value->data, datasize, &enc_data);
	// realloc with size of enc_size and enc_size :
	buf_size += (sizeof(datasize_htonl) + enc_size);
	if ((*msg_buff = realloc(*msg_buff, buf_size)) == NULL) return -1;
	// copy datasize to buffer
	datasize_htonl = htonl(enc_size);
	memcpy(*msg_buff + buf_count, &datasize_htonl, sizeof(datasize_htonl));
	buf_count += sizeof(datasize_htonl);
	// copy data encoded to buffer
	strncpy(*msg_buff + buf_count, enc_data, enc_size);
	
	return buf_size;
}

/*Funcao que coloca a Keys de uma mensagem no buffer*/
int buffer_keys(struct message_t *msg, char **msg_buff){

	int numChars, numKeys = 0;
	while(msg->content.keys[numKeys] != NULL){
		numChars += strlen(msg->content.keys[numKeys]);
		numKeys++;
	}
	int size =2+2+4+(2*numKeys)+numChars;
	*msg_buff = (char *)malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}

	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	numKeys = htonl(numKeys);
	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	memcpy(*msg_buff+4, &numKeys, 4);

	int index = 8, i = 0;
	short contador = 0, contador2 = 0;
	while(msg->content.keys[i] != NULL){
		contador = strlen(msg->content.keys[i]);
		contador2 = htons(contador);
		memcpy(*msg_buff+index, &contador2, 2);
		index += 2;
		strncpy(*msg_buff+index, msg->content.keys[i], contador);
		index += contador;
		i++;
	}
	return size;
}

/*Funcao que coloca o Result de uma mensagem no buffer*/
int buffer_result(struct message_t *msg, char **msg_buff){

	int size = 2+2+4;
	*msg_buff = (char *) malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	int result = htonl(msg->content.result);

	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	memcpy(*msg_buff+4, &result, 4);
	return size;
}

/*Funcao que passa apenas o OP_CODE e C_TYPE para o buffer*/
int buffer_none(struct message_t *msg, char **msg_buff){

	int size = 2+2;
	*msg_buff = (char *) malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);

	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	return size;
}

int message_to_buffer(struct message_t *msg, char **msg_buf){

	if(msg == NULL || msg_buf == NULL){
		return -1;
	}

	if(msg->opcode == OP_ERROR){
		int size = 2+2;
		*msg_buf = (char *)malloc(size);
		if(*msg_buf == NULL){
			return -1;
		}
		short opcode = htons(msg->opcode);
		short ctype = htons(msg->c_type);
		memcpy(*msg_buf, &opcode, 2);
		memcpy(*msg_buf+2, &ctype, 2);
		return size;
	}
	
	switch(msg->c_type){
		case CT_KEY :

			return buffer_key(msg, msg_buf);

		case CT_VALUE :

			return buffer_value(msg, msg_buf);

		case CT_ENTRY :

			return buffer_entry(msg, msg_buf);

		case CT_KEYS :

			return buffer_keys(msg, msg_buf);

		case CT_RESULT :

			return buffer_result(msg, msg_buf);
		
		case CT_NONE :
			
			return buffer_none(msg, msg_buf);
		default :
			printf("L254 ; message.c %d\n", msg->c_type);
			printf(" INVALID C_TYPE\n");
			return -1;
	}
}

/*Funcao que le e coloca uma Key do buffer numa mensagem*/
int msg_key(struct message_t *msg, char *msg_buff){
	int buff_count = 4;
	msg->content.key = get_key_from_buffer(msg_buff, &buff_count);
	return 0;
}

/*Funcao que le e coloca uma Value do buffer numa mensagem*/
int msg_value(struct message_t *msg, char *msg_buff){
	int datasize_from_buffer = 0, buff_count = 4;
	// copy datasize from buffer
	memcpy(&datasize_from_buffer, msg_buff + buff_count, sizeof(datasize_from_buffer));
	buff_count += sizeof(datasize_from_buffer);
	// convert datasize from network
	int datasize = ntohl(datasize_from_buffer);
	// if get returned no data
	if(datasize == 0) {
		if ((msg->content.value = data_create(1)) == NULL) {
			return -1;
		}
		msg->content.value->datasize = 0;
		return 0;
	}
	// alloc size for data with base64
	char *data_w_base = NULL;
	if((data_w_base = (char*) malloc(datasize)) == NULL) {
		return -1;
	}
	strncpy(data_w_base, msg_buff + buff_count, datasize);
	// debase64
	char *data = NULL;
	if (base64_decode_alloc(data_w_base, datasize, &data, NULL) == -1) {
		free(data_w_base);
		return -1;
	}
	free(data_w_base);
	// create value struct
	msg->content.value = NULL;
	if ((msg->content.value = data_create2(strlen(data), data)) == NULL) {
		return -1;
	}
	return 0;
}

/*Funcao que le e coloca uma Entry do buffer numa mensagem*/
int msg_entry(struct message_t *msg, char *msg_buff){
	int buff_count = 4;
	int keysize_buffer = 0, keysize = 0;
	int enc_datasize_from_buffer = 0, enc_datasize = 0;
	int datasize = 0;
	char* key = NULL, *enc_data = NULL, *data = NULL;
	struct data_t *new_data = NULL;

	if ((key = get_key_from_buffer(msg_buff, &buff_count)) == NULL) return -1;

	memcpy(&enc_datasize_from_buffer, msg_buff + buff_count, sizeof(&enc_datasize_from_buffer));
	enc_datasize = ntohl(enc_datasize_from_buffer);
	buff_count += sizeof(enc_datasize_from_buffer);
	
	if ((enc_data = (char*) malloc(enc_datasize)) == NULL) {
		free(key);
		return -1;
	}
	// get value data from buf
	strncpy(enc_data, msg_buff + buff_count, enc_datasize);
	buff_count += enc_datasize;
	// deBASE64
	if (base64_decode_alloc(enc_data, enc_datasize, &data, (size_t *) &datasize) == -1) {
		free(key);
		free(enc_data);
		return -1;
	}
	free(enc_data);
	// create value
	if ((new_data = data_create2(datasize, data)) == NULL) {
		free(key);
		return -1;
	}
	// create entry
	if ((msg->content.entry = entry_create(key, new_data)) == NULL) {
		return -1;
	}

	return 0;
}

/*Funcao que le e coloca o Keys do buffer numa mensagem*/
int msg_keys(struct message_t *msg, char *msg_buff) {

	int buffCount = 4, numKeys;
	memcpy(&numKeys, msg_buff+buffCount, 4);
	numKeys = ntohl(numKeys);
	buffCount += 4;
	if((msg->content.keys = malloc((numKeys+1)*sizeof(char *))) == NULL){
		return -1;
	}

	int i = 0;
	short currSize = 0;
	while(i < numKeys){
		memcpy(&currSize, msg_buff+buffCount, 2);
		currSize = ntohs(currSize);
		buffCount += 2;
		msg->content.keys[i] = (char *) malloc(currSize+1);
		strcpy(msg->content.keys[i], msg_buff+buffCount);
		buffCount+=currSize;
		i++;
	}
	msg->content.keys[i] = NULL;
	return 0;
}

/*Funcao que le e coloca o Result do buffer numa mensagem*/
int msg_result(struct message_t *msg, char *msg_buff) {
	memcpy(&msg->content.result, msg_buff+4,4);
	msg->content.result = ntohl(msg->content.result);
	return 0;
}

struct message_t *buffer_to_message(char *msg_buf, int msg_size){
	if (msg_buf == NULL) {
		// printf("DEBUG MSG_BUF É NULL\n");
		return NULL;
	}

	if (msg_size < 0) {
		// printf("DEBUG MSG SIZE < 0\n");
		return NULL;
	}

	struct message_t *newMessage = (struct message_t *) malloc(sizeof(struct message_t));
	if(newMessage == NULL){
		// printf("DEBUG ERRO A ALOCAR MEM\n");
		return NULL;
	}
	memcpy(&newMessage->opcode, msg_buf, 2);
	newMessage->opcode = ntohs(newMessage->opcode);
	memcpy(&newMessage->c_type, msg_buf+2, 2);
	newMessage->c_type = ntohs(newMessage->c_type);

	if(newMessage->opcode == OP_ERROR){
		return newMessage;
	}

	switch(newMessage->c_type){
		case CT_KEY :
			if(msg_key(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_KEY+1 :
			if(msg_key(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_VALUE :
			if(msg_value(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_ENTRY :
			if(msg_entry(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_KEYS :
			if(msg_keys(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_RESULT :
			if(msg_result(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;

		case CT_NONE :
			return newMessage;

		default :
			return NULL;
	}
}


void free_message (struct message_t *msg){
	if(msg == NULL){
		return;
	}
	int i = 0;
	switch (msg->c_type){
		case CT_KEY :
			free(msg->content.key);
			free(msg);
			break;

		case CT_VALUE :
			data_destroy(msg->content.value);
			free(msg);
			break;

		case CT_ENTRY :
			entry_destroy(msg->content.entry);
			free(msg);
			break;

		case CT_KEYS :
			while(msg->content.keys[i] != NULL){
				free(msg->content.keys[i]);
				i++;
			}
			free(msg->content.keys[i]);
			free(msg->content.keys);
			free(msg);
			break;

		case CT_RESULT :
			free(msg);
			break;

		case CT_NONE :
			free(msg);
			break;

		default :
			printf("L486 ; message.c ; %d\n", msg->c_type);
			printf(" INVALID C_TYPE\n");
	}
}

void print_message(struct message_t *msg) {
    int i;
    
    printf("\n----- MESSAGE -----\n");
    printf("opcode: %d, c_type: %d\n", msg->opcode, msg->c_type);
    switch(msg->c_type) {
        case CT_ENTRY:{
            printf("key: %s\n", msg->content.entry->key);
            printf("datasize: %d\n", msg->content.entry->value->datasize);
            printf("data: %s\n", (char*) msg->content.entry->value->data);
        }break;
        case CT_KEY:{
            printf("key: %s\n", msg->content.key);
        }break;
        case CT_KEYS:{
            for(i = 0; msg->content.keys[i] != NULL; i++) {
                printf("key[%d]: %s\n", i, msg->content.keys[i]);
            }
        }break;
        case CT_VALUE:{
            printf("datasize: %d\n", msg->content.value->datasize);
            printf("data: %s\n", (char*) msg->content.value->data);
        }break;
        case CT_RESULT:{
            printf("result: %d\n", msg->content.result);
        }break;
        case CT_NONE:{
            printf("none\n");
        };
    }
    printf("-------------------\n");
}
