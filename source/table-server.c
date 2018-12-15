/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
/*
   Programa que implementa um servidor de uma tabela hash com chainning.
   Uso: table-server <port> <n_lists>
   Exemplo de uso: ./table_server 54321 6
*/
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

#include "network_server.h"
#include "table_skel.h"
#include "persistence_manager.h"
#include "persistent_table.h"

struct ptable_t *ptable;
int KEEP_LOGS = 0;

void close_server(int listening_socket) {
	network_server_close(listening_socket);
	ptable_destroy(ptable);
}

int main(int argc, char **argv){

	unsigned short server_port;
	int listening_socket, n_lists, log_size;
	char *log_filename;

	/* Testar os argumentos de entrada */
	/* testar se o numero de argumentos esta correto */
	if (argc != 6) {
		printf("Usage: table-server <port> <n_lists> <log-filename> <log-size> <keep-logs>\n");
		printf("Keep logs: >= 1 keeps log and checkpoint file\n");
		printf("Exemple: ./table_server 54321 6 server-logs 64000 1\n");
		return -1;
	}

	server_port = atoi(argv[1]);
	n_lists = atoi(argv[2]);
	if (server_port <= 1024 || server_port > 65535) {
		perror("Server port must be higher than 1024 and lower than 65535");
		return -1;
	}
	if (n_lists <= 0) {
		perror("Number of lists must be larger than 0");
		return -1;
	}
	log_filename = argv[3];
	log_size = atoi(argv[4]);
	if (log_size <= 0) {
		perror("Log size must be higher than 0!");
		return -1;
	}
	KEEP_LOGS = atoi(argv[5]);

	/* inicialização da camada de rede */
	listening_socket = network_server_init(server_port);
	/* inicializacao da tabela e respetivas listas */
	if(table_skel_init(n_lists, log_filename, log_size) == -1) {
		return -1;
	}
	printf("Criou %d tabelas..\n", n_lists);

	/* server loop */
	network_main_loop(listening_socket);

	printf("Loop finished...\n");
	
	close_server(listening_socket);
}
