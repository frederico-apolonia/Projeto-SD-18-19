#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "base64.h"

int message_to_buffer(struct message_t *msg, char **msg_buf){
	
	//VALIDAR OPCODE EM MSG
	if(msg == NULL || msg_buf != NULL){
		return -1;
	}
	
	switch(msg->c_type){
		case CT_KEY :
			return buffer_key(msg, msg_buf);

		case CT_KEY+1 :
			return buffer_key(msg, msg_buf);
			
		case CT_VALUE :
			return buffer_value(msg, msg_buf);

		case CT_VALUE+1 :
			return buffer_value(msg, msg_buf);
			
		case CT_ENTRY :
			return buffer_entry(msg, msg_buf);

		case CT_ENTRY+1 :
			return buffer_entry(msg, msg_buf);
			
		case CT_KEYS :
			return buffer_keys(msg, msg_buf);
			
		case CT_KEYS+1 :
			return buffer_keys(msg, msg_buf);
		
		case CT_RESULT :
			return buffer_result(msg, msg_buf);
			
		case CT_RESULT+1 :
			return buffer_result(msg, msg_buf);
		default :
			printf("INVALID C_TYPE\n");
			return -1;
	}
}

int buffer_key(struct message_t *msg, char **msg_buff){
	//OPCODE + CTYPE + KEYSIZE + KEY
	short keysize = strlen(msg->content.key);
	int size = 2+2+2+keysize;
	msg_buff = malloc(size);
	if(msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	keysize = htons(keysize);
	memcpy(msg_buff, &opcode, 2);
	memcpy(msg_buff+2, &ctype, 2);
	memcpy(msg_buff+4, &keysize, 2);
	memcpy(msg_buff+6, msg->content.key, ntohs(keysize));
	return size;
}

int buffer_value(struct message_t *msg, char **msg_buff){
	int datasize = msg->content.value->datasize;
	int size = 2+2+4+datasize;
	msg_buff = malloc(size);
	if(msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	memcpy(msg_buff, &opcode, 2);
	memcpy(msg_buff+2, &ctype, 2);
	msg_buff[4] = htonl(datasize);
	memcpy(msg_buff[8], msg->content.value->data, datasize);
	return size;
}

int buffer_entry(struct message_t *msg, char **msg_buff) {
	
	short keysize = strlen(msg->content.entry->key);
	int datasize = msg->content.entry->value->datasize;
	int size = 2+2+2+keysize+4+datasize;
	msg_buff = malloc(size);
	if(msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	memcpy(msg_buff, &opcode, 2);
	memcpy(msg_buff+2, &ctype, 2);
	msg_buff[4] = htons(keysize);
	memcpy(msg_buff[6], msg->content.entry->key, keysize);
	msg_buff[6+keysize] = htonl(datasize);
	memcpy(msg_buff[6+keysize+4], msg->content.entry->value->data, datasize);
	return size;
}

int buffer_keys(struct message_t *msg, char **msg_buff){
	
	int numChars, numKeys = 0;
	while(msg->content.keys[numKeys] != NULL){
		numChars += strlen(msg->content.keys[numKeys]);
		numKeys++;
	}
	int size =2+2+4+(2*numKeys)+numChars;
	msg_buff = malloc(size);
	if(msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	numKeys = htonl(numKeys);
	memcpy(msg_buff, &opcode, 2);
	memcpy(msg_buff+2, &ctype, 2);
	memcpy(msg_buff+4, &numKeys, 4);
	
	int index = 8, i = 0;
	short contador = 0, contador2 = 0;
	while(msg->content.keys[i] != NULL){
		contador = strlen(msg->content.keys[i]);
		contador2 = htons(contador);
		memcpy(msg_buff+index, &contador2, 2);
		index += 2;
		memcpy(msg_buff+index, msg->content.keys[i], contador);
		index += contador;
	}
	return size;
}

int buffer_result(struct message_t *msg, char **msg_buff){
	
	int size = 2+2+4;
	msg_buff = malloc(size);
	if(msg_buff == NULL){
		return -1;
	}
	short opcode = htons(msg->opcode);
	short ctype = htons(msg->c_type);
	int result = htonl(msg->content.result);
	memcpy(msg_buff, &opcode, 2);
	memcpy(msg_buff+2, &ctype, 2);
	memcpy(msg_buff+4, &result, 4);
	return size;
}

struct message_t *buffer_to_message(char *msg_buf, int msg_size){
	struct message_t *newMessage = malloc(sizeof(struct message_t *));
	if(newMessage == NULL){
		return NULL;
	}
	//REDO======================================================
	newMessage->opcode = ntohs(msg_buf[0]);
	newMessage->c_type = ntohs(msg_buf[2]);
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

		case CT_VALUE+1 :
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

		case CT_ENTRY+1 :
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
		
		case CT_KEYS+1 :
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
			
		case CT_RESULT+1 :
			if(msg_result(newMessage, msg_buf) == -1){
				free(newMessage);
				return NULL;
			}
			return newMessage;
			
		default :
			printf("INVALID C_TYPE\n");
			return NULL;
	}

}

int msg_key(struct message_t *msg, char **msg_buff){
	
	int keysize;
	memcpy(&keysize, test+4, 2);
	keysize = ntohs(keysize);
	if((msg->content.key = malloc(keysize)) == NULL) {
		return -1;
	}
	memcpy(msg->content.key, msg_buff+6, keysize);
	return 0;
}

int msg_value(struct message_t *msg, char **msg_buff){
	
	int datasize = ntohl(msg_buff[4]);
	char *val = malloc(datasize);
	if(val == NULL){
		return -1;
	}
	memcpy(val, msg_buff[8], datasize);
	if((msg->content.value = data_create2(datasize,val))== NULL){
		free(val);
		return -1;
	}
	return 0;
}

int msg_entry(struct message_t *msg, char **msg_buff){
	
	int keysize = ntohs(msg_buff[4]);
	char *key = malloc(keysize);
	if(key == NULL){
		return -1;
	}
	memcpy(key, msg_buff[6], keysize);
	int datasize = ntohs(msg_buff[6+keysize]);
	char *val = malloc(datasize);
	if(val == NULL){
		free(key);
		return -1;
	}
	memcpy(val, msg_buff[6+keysize+4], datasize);
	struct data_t *newData = data_create2(datasize, val);
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
	}
	return 0;
}

int msg_keys(struct message_t *msg, char **msg_buff) {
	
	int numKeys = ntohl(*(msg_buff+4));
	if((msg->content.keys = malloc((numKeys+1)*sizeof(char *))) == NULL){
		return -1;
	}
	
	int index = 8, i = 0;
	short currSize = 0;
	while(i < numKeys){
		currSize = ntohs(*(msg_buff+index));
		index+=2;
		msg->content.keys[i] = malloc(currSize);
		memcpy(msg->content.keys[i], msg_buff+index, currSize);
		index+=currSize;
		i++;
	}
	msg->content.keys[i] = malloc(4);
	msg->content.keys[i] = NULL;
	return 0;
}

int msg_result(struct message_t *msg, char **msg_buff) {
	msg->content.result = ntohl(*(msg_buff+4));
	return 0;
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
			return;
			
		case CT_KEY+1 :
			free(msg->content.key);
			free(msg);
			return;
			
		case CT_VALUE :
			data_destroy(msg->content.value);
			free(msg);
			return;
			
		case CT_VALUE+1 :
			data_destroy(msg->content.value);
			free(msg);
			return;
		
		case CT_ENTRY :
			entry_destroy(msg->content.entry);
			free(msg);
			return;
			
		case CT_ENTRY+1 :
			entry_destroy(msg->content.entry);
			free(msg);
			return;
			
		case CT_KEYS :
			while(msg->content.keys[i] != NULL){
				free(msg->content.keys[i]);
				i++;
			}
			free(msg->content.keys[i]);
			free(msg->content.keys);
			free(msg);
			return;
		
		case CT_KEYS+1 :
			while(msg->content.keys[i] != NULL){
				free(msg->content.keys[i]);
				i++;
			}
			free(msg->content.keys[i]);
			free(msg->content.keys);
			free(msg);
			return;
			
		case CT_RESULT :
			free(msg);
			return;
			
		case CT_RESULT+1 :
			free(msg);
			return;
			
		default :
			printf("INVALID C_TYPE\n");
			return;
	}
}
