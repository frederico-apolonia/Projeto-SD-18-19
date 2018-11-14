#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "network_client.h"
#include "client_stub-private.h"
#include "read_write.h"

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
		perror("Erro ao conectar-se ao servidor");
		close(rtable->sockfd);
		return -1;
    }
	printf("Connected.\n");
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
		perror ("Erro ao serializar a msg");
		close(rtable->sockfd);
		return NULL;					   
	}
	result = write_all(rtable->sockfd, msg_enviada, size);
	if (result != size){
		perror("Erro ao enviar dados ao servidor");
		close(rtable->sockfd);
		return NULL;
	}
	free(msg_enviada);
	printf("À espera de resposta do servidor ...\n");
	// receber msg do servidor
	if ((size = read_all(rtable->sockfd, &msg_recebida)) < 0) {
		perror("Erro ao receber dados do servidor");
		close(rtable->sockfd);
		return NULL;
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