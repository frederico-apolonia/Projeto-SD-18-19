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
struct table_t *table;

int init_ptable(int n_lists, char *log_filename, int log_size) {
	struct pmanager_t *pmanager;
	/* init table */
	if(table_skel_init(n_lists) == -1) {
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
	if (argc != 5) {
		printf("Usage: table-server <port> <n_lists> <log-filename> <log-size>\n");
		printf("Exemple: ./table_server 54321 6 server-logs 64000\n");
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

	/* inicialização da camada de rede */
	listening_socket = network_server_init(server_port);
	/* inicializacao da tabela e respetivas listas */
	if(init_ptable(n_lists, log_filename, log_size) == -1) {
		return -1;
	}
	printf("Criou %d tabelas..\n", n_lists);

	/* server loop */
	network_main_loop(listening_socket);

	printf("Loop finished...\n");
	
	close_server(listening_socket);
}
