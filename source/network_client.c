/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "network_client.h"
#include "client_stub-private.h"
#include "read_write.h"

extern pthread_mutex_t t_mutex;
extern pthread_mutex_t t_ftry_mutex;
extern int FIRST_TRY;

/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t* rtable){
	
	// Cria socket TCP
    if ((rtable->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket TCP");
        return -1;
    }
	
	// Preenche estrutura server com endereço do servidor para estabelecer
    // conexão
    rtable->sockaddr.sin_family = AF_INET; // família de endereços
    rtable->sockaddr.sin_port = htons(atoi(rtable->port)); // Porta TCP
    //Converte o hostname em string para o formato server.sin_addr
	if (inet_pton(AF_INET, rtable->ip, &rtable->sockaddr.sin_addr) != 1) {
		printf("Erro ao converter IP\n");
		close(rtable->sockfd);
		return -1;
	}
	
	// Estabelece conexão com o servidor definido na estrutura server
    if (connect(rtable->sockfd,(struct sockaddr *) &rtable->sockaddr, sizeof(rtable->sockaddr)) < 0) {
		// perror("DEBUG: Erro ao conectar-se ao servidor");
		close(rtable->sockfd);
		return -1;
    }
	// printf("DEBUG: Thread connected.\n");
	return 0;
     
	
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct message_t *network_send_receive(struct rtable_t * rtable,
                                       struct message_t *msg){
	char *msg_enviada = NULL, *msg_recebida = NULL;
	struct message_t* msg_result = NULL;
	int size, result;
	
	if ((size = message_to_buffer(msg,&msg_enviada)) == -1){
		// perror ("Erro ao serializar a msg");
		close(rtable->sockfd);
		return NULL;					   
	}
	
	pthread_mutex_lock(&t_ftry_mutex);
	int f_try = FIRST_TRY;
	pthread_mutex_unlock(&t_ftry_mutex);

	result = write_all(rtable->sockfd, msg_enviada, 1);
	while(f_try >= 0) {
		result += write_all(rtable->sockfd, msg_enviada+1, size-1);
		if (result != size) {
			// printf("DEBUG: ERRO A ESCREVER MSG\n");
			if (f_try == 0) {
				return NULL;
			}
			// perror("DEBUG (network_client)");
			// printf("DEBUG: Valor resultado escrita %d ; Valor do size: %d\n", result, size);
			// printf("DEBUG: Erro ao enviar dados para o servidor!\n");
			sleep(RETRY_TIME);
			
			network_close(rtable);
			int network_reconnect = network_connect(rtable);

			if(network_reconnect < 0) {
				// printf("DEBUG: Thread nao se conseguiu reconnectar ao server... Bye.\n");
				return NULL;
			} else {
				// printf("DEBUG: Thread reconnected!\n");
			}

			pthread_mutex_lock(&t_ftry_mutex);
			FIRST_TRY--;
			pthread_mutex_unlock(&t_ftry_mutex);
		} else break;
	}
	free(msg_enviada);

	//printf("À espera de resposta do servidor ...\n");
	// receber msg do servidor

	pthread_mutex_lock(&t_ftry_mutex);
	int first_try = FIRST_TRY;
	pthread_mutex_unlock(&t_ftry_mutex);

	while(first_try >= 0) {
		size = read_all(rtable->sockfd, &msg_recebida);
		if (size < 0) {
			// printf("DEBUG: ERRO A RECEBER MSG\n");
			if (first_try == 0) {
				return NULL;
			}
			pthread_mutex_lock(&t_ftry_mutex);
			FIRST_TRY--;
			pthread_mutex_unlock(&t_ftry_mutex);

			// perror("DEBUG (network_client)");
			// printf("DEBUG: Cliente nao recebeu bem...\n");

			sleep(RETRY_TIME);
				
			network_close(rtable);
			int network_reconnect = network_connect(rtable);

			if(network_reconnect < 0) {
				// printf("DEBUG: Thread nao se conseguiu reconnectar ao server... Bye.\n");
				return NULL;
			}
			// printf("DEBUG: Thread reconnected!\n");

		} else break;
	}
	msg_result = buffer_to_message(msg_recebida,size);
	free(msg_recebida);
	return msg_result;
}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtable_t * rtable){
	return close(rtable->sockfd);
}