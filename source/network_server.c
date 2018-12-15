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
#include <poll.h>
#include <fcntl.h>
#include <signal.h>

#include "message.h"
#include "table_skel.h"
#include "network_server.h"
#include "read_write.h"
#include "message-private.h"

static volatile int keep_looping = 1;

#define NFDESC 512
#define TIMEOUT 5000

void sigint_handler(int dummy) {
    keep_looping = 0;
}


/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar o descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){
	int sockfd, optval=1;
	struct sockaddr_in server;
	// creates TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error while creating socket");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (int *)&optval,
      	sizeof(optval)) < 0 ) {
    	perror("SO_REUSEADDR setsockopt error");
  	}

	// fills struct server with addresses to bind to the socket
	server.sin_family = AF_INET;
	server.sin_port = htons(port); // TCP Port
	server.sin_addr.s_addr = htonl(INADDR_ANY); // All machine addresses

	//bind
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Error while binding socket");
		close(sockfd);
		return -1;
	}
	// listening socket
	if (listen(sockfd, 0) < 0){
        perror("Erro ao executar listen");
        close(sockfd);
        return -1;
    }

	// now that the socket is created and binded, return
	return sockfd;
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){

	if  (listening_socket < 0) {
		return -1;
	}
	struct pollfd *connections = (struct pollfd*)malloc(sizeof(struct pollfd)*NFDESC);
	if(connections == NULL){
		return -1;
	}
	struct sockaddr_in client;
	socklen_t size_client = sizeof(struct sockaddr);
	int client_socket, msg_process_result, send_msg_result, result, closed = 0, nfds, kfds, i,j;
	int current_size,end_server = 1,new_sd = -1;
	char cmessage,c;

	for (i = 0; i < NFDESC; i++){
    		connections[i].fd = -1;
	}

 	connections[0].fd = listening_socket;  // Vamos detetar eventos na welcoming socket
 	connections[0].events = POLLIN;

  	nfds = 1;

	signal(SIGINT, sigint_handler);
	
	while((kfds = poll(connections, nfds, TIMEOUT)) >= 0 && keep_looping){
		//caso haja uma nova conexao
		if((connections[0].revents & POLLIN) && (nfds < NFDESC)){
			//coloca-se o cliente em connections
			if ((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *) &client, &size_client)) > 0){
				connections[nfds].events = POLLIN;
				nfds++;
			}
		}
		
		for(i = 1; i < nfds; i++ ){
			if(recv(connections[i].fd, &c, 1, MSG_PEEK | MSG_DONTWAIT) == 0){
						// printf("CONNECTION CLOSED\n");
						closed = 1;
						close(connections[i].fd);
                        connections[i].fd = -1;
                        connections[i].events = 0;
                        nfds--;
                        break;
            }
			if(connections[i].revents & POLLIN) {
				struct message_t* message;
				if ((message = network_receive(connections[i].fd)) == NULL) {
					
				}else{
					//printf("DEBUG: New message received from the client...\n");
					//print_message(message); // for debugging purposes
					// invoke will update message, returns -1 if something fails
					msg_process_result = invoke(message);
					if (msg_process_result == -1) {
						// printf("DEBUG: There was an error while processing the current message\n");
						build_error_message(message);
						// printf("DEBUG: error message:\n");
						// print_message(message);
					}
					//printf("DEBUG: Message that is going to be sent to the client:\n");
					//print_message(message);
					if ((send_msg_result = network_send(connections[i].fd, message)) > 0) {
						//printf("DEBUG: Message was successfuly processed and sent to client\n");
					} else {
						//caso ocorra erro de envio de mensagem, fecha-se o socket do cliente
						/*printf("DEBUG: Message send failed, closing conn\n");
						close(connections[i].fd);
						connections[i].fd = -1;
						closed = 1;
						break;*/
					}
				}
				free_message(message);
			}

			if((connections[i].revents & POLLHUP) || (connections[i].revents & POLLERR)){
				close(connections[i].fd);
				connections[i].fd = -1;
				closed = 1;
				break;
			}
		}
		//se houver fecho de alguma conexao, as posicoes dos clientes
		//vao ser shifted para a esquerda
		if(closed){
			closed = 0;
			int j, k;
			for(j = 0; j < nfds; j++){
				// printf("DEBUG: CONNECTIONS.FD = %d | POS: %d\n",connections[j].fd,j);
				if(connections[j].fd == -1){
					for(k = j; k < nfds; k++){
						connections[k].fd = connections[k+1].fd;
					}
					j--;
					nfds--;
				}
			}
		}
	}
	free(connections);
	close(listening_socket);
	return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
	struct message_t *message = NULL;
	int result = 0;
	char *buffer = NULL;

	result = read_all(client_socket, &buffer);
	if(result < 0) {
		/* ocorreu um erro a ler a resposta */
		// printf("DEBUG: (network_server) Erro a ler do cliente\n");
		free(buffer);
		close(client_socket);
		return NULL;
	} else {/* processamento da requisição e da resposta */
		message = buffer_to_message(buffer, result);
		free(buffer);
		return message;
	}

	return NULL;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg) {
	int msg_size = 0, size_to_client, function_result;
	char* buffer;

	if((msg_size = message_to_buffer(msg, &buffer)) < 0 ) {
		perror("MESSAGE SERIALIZATION FAILED");
		free(buffer);
		return -1;
	}
	// sends message to client
	if ((function_result = write_all(client_socket, buffer, msg_size)) != msg_size) {
		perror("Failed to send message to client");
		close(client_socket);
		free(buffer);
		return -1;
	}
	free(buffer);
	return function_result;
}

/* A função network_server_close() fecha a ligação estabelecida por
 * network_server_init().
 */
int network_server_close(int listening_socket){
	close(listening_socket);
}
