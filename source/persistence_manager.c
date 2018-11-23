#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include "persistence_manager.h"
#include "persistence_manager-private.h"
#include "read_write.h"
#include "message.h"

/* Cria um gestor de persist�ncia que armazena logs em filename+".log" e o
 * estado do sistema em filename+".ckp". O par�metro logsize define o
 * tamanho m�ximo em bytes que o ficheiro de log pode ter e o par�metro
 * filename pode ser um path completo.
 * Retorna o pmanager criado ou NULL em caso de erro.
 */
struct pmanager_t *pmanager_create(char *filename, int logsize){

	if(filename == NULL){
		perror("Filename is NULL");
		return NULL;
	}

	if(logsize <= 0){
		perror("Invalid logsize");
		return NULL;
	}

	struct pmanager_t *pmanager = (struct pmanager_t *) malloc(sizeof(struct pmanager_t));
	if(pmanager == NULL){
		return NULL;
	}

	if((pmanager->filename = strdup(filename)) == NULL){
		perror("Filename malloc error");
		return NULL;
	}

	//malloc -> nome do ficheiro + terminacao .log + \0
	if((pmanager->logfilename = malloc(strlen(filename)*sizeof(char) + 4 + 1)) == NULL){
		perror("Log filename malloc error");
		free(pmanager);
		return NULL;
	}

	if((pmanager->ckpfilename = malloc(strlen(filename)*sizeof(char) + 4 + 1)) == NULL){
		perror("Ckp filename malloc error");
		free(pmanager->logfilename);
		free(pmanager);
		return NULL;
	}

	//CHECK
	strcpy(pmanager->logfilename, filename);
	strcat(pmanager->logfilename, ".log");

	strcpy(pmanager->ckpfilename, filename);
	strcat(pmanager->ckpfilename, ".ckp");

	if((pmanager->logfile = open(pmanager->logfilename,O_WRONLY|O_CREAT,0640)) < 0){
		perror("Log open error");
		free(pmanager->logfilename);
		free(pmanager->ckpfilename);
		free(pmanager);
		return NULL;
	}

	if((pmanager->ckpfile = open(pmanager->ckpfilename,O_WRONLY|O_CREAT,0640)) < 0){
		perror("Ckp open error");
		close(pmanager->logfile);
		free(pmanager->logfilename);
		free(pmanager->ckpfilename);
		free(pmanager);
		return NULL;
	}

	pmanager->sttfilename = NULL;
	pmanager->sttfile = 0;
	pmanager->logsize = logsize;
	pmanager->currsize = 0;
}

/* Destr�i o gestor de persist�ncia passado como par�metro.
 * Esta fun��o n�o limpa os ficheiros de log e ckp do sistema.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy(struct pmanager_t *pmanager){
	if(pmanager == NULL){
		return -1;
	}

	if(pmanager->filename != NULL){
		free(pmanager->filename);
	}

	if(pmanager->logfilename != NULL){
		free(pmanager->logfilename);
	}

	if(pmanager->ckpfilename != NULL){
		free(pmanager->ckpfilename);
	}

	if(pmanager->sttfilename != NULL){
		free(pmanager->sttfilename);
	}

	if(pmanager->logfile > 0){
		close(pmanager->logfile);
	}

	if(pmanager->ckpfile > 0){
		close(pmanager->ckpfile);
	}
	free(pmanager);
	return 0;
}

/* Faz o mesmo que a fun��o pmanager_destroy, mas apaga os ficheiros
 * de log e ckp geridos por este gestor de persist�ncia.
 * Retorna 0 se tudo estiver OK ou -1 em caso de erro.
 */
int pmanager_destroy_clear(struct pmanager_t *pmanager){

	if(pmanager == NULL){
		return -1;
	}

	//remocao dos ficheiros log e ckp dado o nome do ficheiro
	if(remove(pmanager->logfilename) < 0){
		return -1;
	}

	if(remove(pmanager->ckpfilename) < 0){
		return -1;
	}

	if(pmanager->filename != NULL){
		free(pmanager->filename);
	}

	if(pmanager->logfilename != NULL){
		free(pmanager->logfilename);
	}

	if(pmanager->ckpfilename != NULL){
		free(pmanager->ckpfilename);
	}

	if(pmanager->sttfilename){
		free(pmanager->sttfilename);
	}

	if(pmanager->logfile > 0){
		close(pmanager->logfile);
	}

	if(pmanager->ckpfile > 0){
		close(pmanager->ckpfile);
	}

	free(pmanager);

	return 0;
}

/* Verifica se os ficheiros de log e/ou ckp cont�m dados.
 * Retorna 1 caso existam dados nos ficheiros de log e/ou ckp e 0
 * caso contr�rio.
 */
int pmanager_have_data(struct pmanager_t *pmanager){
	if(pmanager == NULL){
		return -1;
	}

	int result = 0;
	if(pmanager->logfile > 0){
		if(lseek(pmanager->logfile, 0, SEEK_END) > 0){
			result = 1;
		}
	}

	if(pmanager->ckpfile > 0){
		if(lseek(pmanager->ckpfile, 0, SEEK_END) > 0){
			result = 1;
		}
	}

	return result;
}

/* Escreve a opera��o op no fim do ficheiro de log associado a pmanager,
 * mas apenas se a dimens�o do ficheiro, ap�s a escrita, n�o ficar maior
 * que logsize. A escrita deve ser feita com o modo de escrita definido.
 * Retorna o n�mero de bytes escritos no log ou -1 no caso do ficheiro
 * j� n�o ter espa�o suficiente para registar a opera��o.
 */
int pmanager_log(struct pmanager_t *pmanager, struct message_t *op){
	if(pmanager == NULL || op == NULL) {
		return -1;
	}
	if(pmanager->logfile < 0){
		return -1;
	}

	int filesize, size;
	char *msg_buf;
	size = message_to_buffer(op, &msg_buf);
	if(size < 0){
		perror("Message serialization failed");
		return -1;
	}
	if(pmanager->logfile >= 0){
		if(pmanager->currsize+size > pmanager->logsize){
			free(msg_buf);
			return -1;
		}else{
  		pmanager->logfile = open(pmanager->logfilename, O_WRONLY | O_APPEND);
			if(pmanager->logfile < 0){
				perror("Unable to open log");
				free(msg_buf);
				return -1;
			}
			int write_result = write_all(pmanager->logfile, msg_buf, size);
			if(write_result < size){
				perror("Read fail");
				return -1;
			}
			free(msg_buf);
			pmanager->currsize += size;
			return size;
		}
	}else{
  		free(msg_buf);
		return -1;
	}
}

/*Copia os conteudos da table para o ficheiro
  retorna 0 em caso de sucesso e -1 caso contrario*/
int table_write_stt(int fd, struct table_t *table){

	char** keys = table_get_keys(table);
	char* curr_buff;
	struct entry_t* curr_entry;
	struct data_t* curr_data;
	struct message_t* curr_message;
	int i, buff_size, fsize;

	for(i = 0; i < table->numElems; i++){
		//retira uma copia do data na tabela
		if((curr_data = table_get(table, keys[i])) == NULL){
			return -1;
		}
		//cria uma entry com a copia
		if((curr_entry = entry_create(keys[i], curr_data)) == NULL){
			return -1;
		}

		if((curr_message = (struct message_t*) malloc(sizeof(struct message_t))) == NULL){
			return -1;
		}
		//colocacao de campos predefinidos no message e a entry criada
		curr_message->opcode = OP_PUT;
		curr_message->c_type = CT_ENTRY;
		curr_message->content.entry = curr_entry;
		if((buff_size = message_to_buffer(curr_message, &curr_buff)) < 0){
			free_message(curr_message);
			return -1;
		}

		//escrita para o ficheiro
		if(write_all(fd, curr_buff, buff_size) < buff_size){
			free_message(curr_message);
			perror("stt write error");
			return -1;
		}
		fsize += buff_size;
		//libertacao de memoria
		//entry_destroy(curr_entry);
		free_message(curr_message);
		free(curr_buff);
	}
	table_free_keys(keys);
	return fsize;
}

/* Cria um ficheiro filename+".stt" com o estado de table.
 * Retorna o tamanho do ficheiro criado ou -1 em caso de erro.
 */
int pmanager_store_table(struct pmanager_t *pmanager, struct table_t *table) {

	if(pmanager == NULL){
		perror("Pmanager is NULL");
		return -1;
	}

	if(table == NULL){
		perror("Table is NULL");
		return -1;
	}

	if((pmanager->sttfilename = malloc(strlen(pmanager->filename)*sizeof(char) + 4 + 1)) == NULL){
		return -1;
	}

	strcpy(pmanager->sttfilename, pmanager->filename);
	strcat(pmanager->sttfilename, ".stt");

	if((pmanager->sttfile = open(pmanager->sttfilename, O_WRONLY|O_CREAT,0640)) < 0) {
		perror("Open error");
		return -1;
	}

	int write_size = table_write_stt(pmanager->sttfile, table);
	if( write_size < 0 ){
		perror("Stt write error");
		return -1;
	}

	return write_size;
}

/* Limpa o ficheiro ".log" e copia o ficheiro ".stt" para ".ckp".
 * Retorna 0 se tudo correr bem ou -1 em caso de erro.
 */
int pmanager_rotate_log(struct pmanager_t *pmanager){

	if(pmanager == NULL){
		perror("Pmanager is NULL");
		return -1;
	}

	if(pmanager->ckpfile < 0 || pmanager->logfile < 0 || pmanager->sttfile < 0){
		perror("Invalid file");
		return -1;
	}

	pmanager->sttfile = open(pmanager->sttfilename, O_RDONLY);
	if(pmanager->sttfile < 0){
		perror("Unable to open file");
		return -1;
	}
	//apagar ckp ou append?
	pmanager->ckpfile = open(pmanager->ckpfilename, O_WRONLY);
	if(pmanager->ckpfile < 0){
		perror("Unable to open file");
		return -1;
	}

	//obter tamanho (bytes) de stt
	int stt_size = lseek(pmanager->sttfile, 0, SEEK_END);
	//coloca o offset no inicio para fazer a leitura de stt
	lseek(pmanager->sttfile, 0, SEEK_SET);

	char* buff = malloc(stt_size*sizeof(char));
	if(buff == NULL){
		return -1;
	}

	//faz a leitura inteira do stt para buff
	if(read(pmanager->sttfile, buff, stt_size) < 0){
		perror("Stt read error");
		free(buff);
		return -1;
	}
	//coloca o offset no inicio, depois do read o ter movido
	lseek(pmanager->sttfile, 0, SEEK_SET);
	//write_all(pmanager->ckpfile, buff, stt_size);

	//escreve buff para ckp
	if(write_all(pmanager->ckpfile, buff, stt_size) < stt_size){
		perror("Ckp write error");
		free(buff);
		return -1;
	}

	pmanager->logfile = open(pmanager->logfilename, O_WRONLY);
	if(pmanager->logfile < 0){
		perror("Unable to open file");
		return -1;
	}
	pmanager->currsize = 0;

	free(buff);
	return 0;
}

//Coloca os dados do backup na tabela
int write_to_table(int fd, struct table_t *table){
	//TODO
	char *buf = NULL;
	int read_result = -1;
	struct message_t *insert = NULL;
	while(1 == 1){
		if (buf != NULL) {
			// limpar o buf
			free(buf);
			buf = NULL;
		}
		read_result = read_all(fd, &buf);
		if (read_result == -1) {
			break;
		}
		insert = buffer_to_message(buf, read_result);
		if(table_put(table, strdup(insert->content.entry->key), data_dup(insert->content.entry->value)) == -1) {
			free_message(insert);
			free(buf);
			return -1;
		}
	}
	return 0;
}


/* Mete o estado contido nos ficheiros .log, .stt e/ou .ckp na tabela
 * passada como argumento. Retorna 0 (ok) ou -1 em caso de erro.
 */
int pmanager_fill_state(struct pmanager_t *pmanager, struct table_t *table) {
	if(pmanager == NULL){
		perror("Pmanager is NULL");
		return -1;
	}

	if(table == NULL){
		perror("Table is NULL");
		return -1;
	}

	//Verifica se log tem conteudos
	if((pmanager->logfile >= 0) && lseek(pmanager->logfile, 0, SEEK_END) > 0){
		lseek(pmanager->logfile, 0, SEEK_SET);
		write_to_table(pmanager->logfile, table);
	}

	//Verifica se ckp tem conteudos
	if((pmanager->ckpfile >= 0) && lseek(pmanager->ckpfile, 0, SEEK_END) > 0){
		lseek(pmanager->ckpfile, 0, SEEK_SET);
		write_to_table(pmanager->ckpfile, table);
	}

	//Verifica se stt tem conteudos
	if((pmanager->sttfile >= 0) && lseek(pmanager->sttfile, 0, SEEK_END) > 0){
		lseek(pmanager->sttfile, 0, SEEK_SET);
		write_to_table(pmanager->sttfile, table);
	}
}
