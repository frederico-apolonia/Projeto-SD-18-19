#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "message.h"
#include "base64.h"
#include "message-private.h"

/* Le uma chave do buffer */
char* get_key_from_buffer(char *msg_buff, int *buff_count) {
	short keysize;
	memcpy(&keysize, msg_buff+*buff_count, sizeof(keysize));
	*buff_count += sizeof(keysize);

	char *key = malloc(keysize + 1);
	if(key == NULL){
		return NULL;
	}
	strncpy(key, msg_buff+*buff_count, ntohs(keysize));
	key[ntohs(keysize)] = '\0';

	*buff_count += ntohs(keysize);
	
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
int buffer_value(struct message_t *msg, char **msg_buff){

	int datasize = msg->content.value->datasize;
	char *encData;
	int encodeSize = base64_encode_alloc(msg->content.value->data, datasize, &encData);
	
	int size = 2+2+4+encodeSize+1;
	*msg_buff = malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	int ds2 = htonl(encodeSize);
	memcpy(*msg_buff+4, &ds2, 4);
	memcpy(*msg_buff+8, encData, encodeSize);
	free(encData);
	return size;
}

/*Funcao que coloca a Entry de uma mensagem no buffer*/
int buffer_entry(struct message_t *msg, char **msg_buff) {
	
	//Versao com BASE64 encode
	/*
	short keysize = strlen(msg->content.entry->key);
	int datasize = msg->content.entry->value->datasize;
	char *encData;
	int encodeSize = base64_encode_alloc(msg->content.value->data, datasize, &encData);
	
	int size = 2+2+2+keysize+1+4+encodeSize;
	*msg_buff = (char *)malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	memcpy(*msg_buff, &opcode, 2);
	memcpy(*msg_buff+2, &ctype, 2);
	short ks2 = htons(keysize);
	memcpy(*msg_buff+4, &ks2, 2);
	memcpy(*msg_buff+6, msg->content.entry->key, keysize);
	int ds2 = htonl(encodeSize);
	memcpy(*msg_buff+6+keysize, &ds2, 4);
	memcpy(*msg_buff+6+keysize+4, encData, datasize);
	free(encData);*/
	
	short keysize = strlen(msg->content.entry->key);
	int datasize = msg->content.entry->value->datasize - 1;
	int size = 2+2+2+keysize+4+datasize;
	*msg_buff = (char *)malloc(size);
	if(*msg_buff == NULL){
		return -1;
	}
	
	short opcode = htons(msg->opcode);
	memcpy(*msg_buff, &opcode, 2);
	short ctype = htons(msg->c_type);
	memcpy(*msg_buff+2, &ctype, 2);
	short ks2 = htons(keysize);
	memcpy(*msg_buff+4, &ks2, 2);
	memcpy(*msg_buff+6, msg->content.entry->key, keysize);
	int ds2 = htonl(datasize);
	memcpy(*msg_buff+6+keysize, &ds2, 4);
	strncpy(*msg_buff+6+keysize+4, msg->content.entry->value->data, datasize);
	return size;
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
	
	int datasize, buffCount = 4;
	memcpy(&datasize, msg_buff+buffCount,4);
	buffCount += 4;
	datasize = ntohl(datasize);
	char *val = malloc(datasize+1);
	if(val == NULL){
		return -1;
	}
	strcpy(val, msg_buff+buffCount);

	char *valDec;
	base64_decode_alloc(val, datasize, &valDec, NULL);
	free(val);

	if((msg->content.value = data_create2(datasize,valDec))== NULL){
		free(val);
		return -1;
	}
	return 0;
}

/*Funcao que le e coloca uma Entry do buffer numa mensagem*/
int msg_entry(struct message_t *msg, char *msg_buff){
	//VERSAO SEM BASE64 DECODE
	
	int buffCount = 4;
	char* key = get_key_from_buffer(msg_buff, &buffCount);

	int datasize;
	memcpy(&datasize, msg_buff+buffCount, 4);
	buffCount += 4;
	
	char *val = malloc(ntohl(datasize)+1);
	if(val == NULL){
		free(key);
		return -1;
	}

	strncpy(val, msg_buff+buffCount, ntohl(datasize));
	val[strlen(val)] = '\0';
	buffCount += ntohl(datasize);

	struct data_t *newData = data_create2(ntohl(datasize)+1, strdup(val));
	if(newData == NULL){
		free(key);
		free(val);
		return -1;
	}

	struct entry_t *entry_result = entry_create(key, newData);
	if(entry_result == NULL){
		free(key);
		free(val);
		data_destroy(newData);
		return -1;
	}
	msg->content.entry = entry_result;
	//Versao com BASE64 decode
	/*
	int buffCount = 4;
	short keysize;
	memcpy(&keysize, msg_buff+buffCount, 2);
	keysize = ntohs(keysize);
	buffCount += 2;
	char *key = malloc(keysize+1);
	if(key == NULL){
		return -1;
	}
	strcpy(key, msg_buff+buffCount);
	buffCount += keysize;

	int datasize;
	memcpy(&datasize, msg_buff+buffCount, 4);
	datasize = ntohl(datasize);
	buffCount += 4;
	
	char *val = malloc(datasize+1);
	if(val == NULL){
		free(key);
		return -1;
	}
	//Buffer corrupto
	strcpy(val, msg_buff+buffCount);
	printf("val: %s\n", val);
	buffCount += datasize;
	
	char *valDec;
	base64_decode_alloc(val, datasize, &valDec, NULL);
	free(val);

	struct data_t *newData = data_create2(datasize, valDec);
	if(newData == NULL){
		free(key);
		free(val);
		return -1;
	}

	if((msg->content.entry = entry_create(key, newData)) == NULL){
		free(key);
		free(val);
		data_destroy(newData);
		return -1;
	}*/
	
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
	struct message_t *newMessage = malloc(sizeof(struct message_t *));
	if(newMessage == NULL){
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
			printf(" INVALID C_TYPE\n");
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
