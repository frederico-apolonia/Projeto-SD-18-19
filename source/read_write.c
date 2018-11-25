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
#include "read_write.h"

/* Le n bytes do buffer do socket para dst */
int read_n(int socket, char* dst, int n) {
	while (n > 0) {
		int r = read(socket, dst, n);
		if (r <= 0) {
			return -1;
		}
		n -= r;
		dst += r;
	}
}

/* Redimensiona o buffer para o tamanho new_buf_size */
int resize_buffer(char **buf, int new_buf_size) {
	if ((*buf = realloc(*buf, new_buf_size)) == NULL) {
		return -1;
	}
	return new_buf_size;
}

/* Adiciona data ao buffer */
int append_into_buffer(char** buf, int* cur_buf_pos, char* data, int size) {
	memcpy(*buf + *cur_buf_pos, data, size);
	*cur_buf_pos += size;
}

/* Le do socket diretamente para o buffer size bytes */
int read_into_buffer(int socket, char* buf, int* cur_buf_pos, int size) {
	if (read_n(socket, buf + *cur_buf_pos, size) == -1) {
		return -1;
	}
	*cur_buf_pos += size;
}

/* Retorna o novo tamanho atual do buffer, -1 se tiver ocorrido algum erro */
int read_key_from_socket(int socket, char** buf, int* buf_size, int* cur_buf_pos) {
	short keysize;
	// le os 2 bytes correspondentes ao tamanho da key
	if (read_n(socket, (char*) &keysize, sizeof(keysize)) == -1) {
		return -1;
	}
	// aumenta o tamanho do buffer para posteriormente escrever o tamanho da key
	if ((*buf_size = resize_buffer(buf, *buf_size + sizeof(keysize))) == -1) {
		return -1;
	}
	// escreve o tamanho da key no buffer
	append_into_buffer(buf, cur_buf_pos, (char *) &keysize, sizeof(keysize));
	// aumenta o tamanho do buffer para buffer+keysize
	if ((*buf_size = resize_buffer(buf, *buf_size + ntohs(keysize))) == -1) {
		return -1;
	}
	// le e escreve no buffer a key
	if (read_into_buffer(socket, *buf, cur_buf_pos, ntohs(keysize)) == -1) {
		return -1;
	}

	return *cur_buf_pos;
}

/* Retorna o novo tamanho atual do buffer, -1 se tiver ocorrido algum erro */
int read_value_from_socket(int socket, char** buf, int* buf_size, int* cur_buf_pos) {
	int datasize;
	// le os 4 bytes correspondentes ao tamanho do data
	if (read_n(socket, (char*) &datasize, sizeof(datasize)) == -1) {
		return -1;
	}
	// aumenta o tamanho do buffer para posteriormente escrever o tamanho do data
	if ((*buf_size = resize_buffer(buf, *buf_size + sizeof(datasize))) == -1) {
		return -1;
	}
	// escreve o tamanho do data no buffer
	append_into_buffer(buf, cur_buf_pos, (char*) &datasize, sizeof(datasize));
	// aumentar tamanho do datasize
	if ((*buf_size = resize_buffer(buf, *buf_size + ntohl(datasize))) == -1) {
		return -1;
	}
	// le e escreve no buffer o data
	if (read_into_buffer(socket, *buf, cur_buf_pos, ntohl(datasize)) == -1) {
		return -1;
	}

	return *cur_buf_pos;
}

int read_n_keys_from_socket(int socket, char** buf, int* buf_size, int* cur_buf_pos) {
	int nkeys;
	// le os 4 bytes correspondentes ao tamanho de nkeys
	if (read_n(socket, (char*) &nkeys, sizeof(nkeys)) == -1) {
		return -1;
	}
	// aumenta o tamanho do buffer para posteriormente escrever o tamanho de nkeys
	if ((*buf_size = resize_buffer(buf, *buf_size + sizeof(nkeys))) == -1) {
		return -1;
	}
	// escreve o tamanho de nkeys no buffer
	append_into_buffer(buf, cur_buf_pos, (char*) &nkeys, sizeof(nkeys));
	// ler n chaves do buffer
	int count = ntohl(nkeys);
	while (count > 0) {
		if ((read_key_from_socket(socket, buf, buf_size, cur_buf_pos)) == -1) {
			return -1;
		}
		count -= 1;
	}
}

/* Le o result do socket */
int read_result_from_socket(int socket, char** buf, int* buf_size, int* cur_buf_pos) {
	int result;
	// le os 4 bytes correspondentes ao tamanho de result
	if (read_n(socket, (char*) &result, sizeof(result)) == -1) {
		return -1;
	}
	// aumenta o tamanho do buffer para posteriormente escrever o tamanho de result
	if ((*buf_size = resize_buffer(buf, *buf_size + sizeof(result))) == -1) {
		return -1;
	}
	// escreve o tamanho de result no buffer
	append_into_buffer(buf, cur_buf_pos, (char*) &result, sizeof(result));
}

/* Lê todos os bytes do socket */
int read_all(int socket, char** buf) {
	int buf_size = 4, curr_buf_pos = 0;
	if((*buf = malloc(buf_size)) == NULL) {
		return -1;
	}
	// escreve o valor de opcode no buffer
	short opcode;
	if (read_n(socket, (char*) &opcode, sizeof(opcode)) == -1) {
		return -1;
	}
	append_into_buffer(buf, &curr_buf_pos, (char*) &opcode, sizeof(opcode));
	// escreve o valor de c_type no buffer
	short c_type;
	if (read_n(socket, (char*) &c_type, sizeof(c_type)) == -1) {
		return -1;
	}

	append_into_buffer(buf, &curr_buf_pos, (char*) &c_type, sizeof(c_type));

	switch (ntohs(c_type))
	{
		case CT_NONE:
			// nao ha mais nada para copiar para o buffer no ct_none
			break;
		case CT_KEY:
			if(read_key_from_socket(socket, buf, &buf_size, &curr_buf_pos) == -1) {
				return -1;
			}
			break;

		case CT_VALUE:
			if(read_value_from_socket(socket, buf, &buf_size, &curr_buf_pos) == -1) {
				return -1;
			}
			break;

		case CT_ENTRY:
			// ler a chave
			if(read_key_from_socket(socket, buf, &buf_size, &curr_buf_pos) == -1) {
				return -1;
			}
			// ler o valor
			if(read_value_from_socket(socket, buf, &buf_size, &curr_buf_pos) == -1) {
				return -1;
			}
			break;

		case CT_KEYS:
			if((read_n_keys_from_socket(socket, buf, &buf_size, &curr_buf_pos)) == -1) {
				return -1;
			}
			break;

		case CT_RESULT:
			if((read_result_from_socket(socket, buf, &buf_size, &curr_buf_pos)) == -1) {
				return -1;
			}
			break;

		default:
			perror("INVALID C_TYPE");
			return -1;
	}
	return buf_size;

}

/* Escreve todos so bytes no socket */
int write_all (int socket, char* buf, int length) {
	int buffer_size = length;
	int write_result;

	while ( length > 0 ) {
		write_result = write(socket, buf, length);
		if( write_result < 0 ) {
			if(errno == EINTR){
				continue;
			}
			perror("Write failed");
			return write_result;
		}
		buf += write_result;
		length -= write_result;
	}
	return buffer_size;
}
