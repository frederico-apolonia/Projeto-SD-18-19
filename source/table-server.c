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

struct table_t * server_table;

int main(int argc, char **argv){

	unsigned short server_port;
	int listening_socket;
	int n_lists, table_init_result;

	/* Testar os argumentos de entrada */
	/* testar se o numero de argumentos esta correto */
	if (argc != 3) {
		printf("Usage: table-server <port> <n_lists>\n");
		printf("Exemple: ./table_server 54321 6\n");
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

	/* inicialização da camada de rede */
	listening_socket = network_server_init(server_port);
	/* inicializacao da tabela e respetivas listas */
	if((table_init_result = table_skel_init(n_lists)) == -1) {
		return -1;
	}
	printf("Criou %d tabelas..\n", n_lists);

	/* server loop */
	network_main_loop(listening_socket);
	
	table_skel_destroy();
	network_server_close(listening_socket);
}
