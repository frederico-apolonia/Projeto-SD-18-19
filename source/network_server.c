#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "message.h"
#include "table_skel.h"
#include "network_server.h"
#include "read_write.h"
#include "message-private.h"

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar o descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){
	int sockfd;
	struct sockaddr_in server;
	// creates TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error while creating socket");
		return -1;
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

	struct sockaddr_in client;
	socklen_t size_client;
	int client_socket;
	int msg_process_result, send_msg_result;
	int result;
	char cmessage;

	printf("Server is waiting for a new connection...\n");
	while ((client_socket = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1){
		printf("New client connection\n");
		while(result = recv(client_socket, &cmessage, sizeof(char), MSG_PEEK)) {
			struct message_t* message;
			// reads message from client
			if ((message = network_receive(client_socket)) != NULL) {
				printf("New message received from the client...\n");
				print_message(message); // for debugging purposes
				// invoke will update message, returns -1 if something fails
				msg_process_result = invoke(message);
				if (msg_process_result == -1) {
					printf("There was an error while processing the current message\n");
					build_error_message(message);
				}
				printf("Message that is going to be sent to the client:\n");
				print_message(message);
				if ((send_msg_result = network_send(client_socket, message)) != -1) {
					printf("Message was successfuly processed and sent to client\n");
				} else {
					printf("There was an error while sending this message to the client.\n");
				}
			}
		}
		printf("Goodbye, client!\n");
		close(client_socket);
	}
	perror("Error from client socket");
	printf("Client socket: %d\n", client_socket);
	return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
	struct message_t *message;
	int result;
	char *buffer;

	result = read_all(client_socket, &buffer);
	if(result < 0) {
		/* ocorreu um erro a ler a resposta */
		free(buffer);
		perror("Reading client buffer failed");
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
	
	free_message(msg);
	// sends message to client
	if ((function_result = write(client_socket, buffer, msg_size)) != msg_size) {
		perror("SEND MESSAGE TO CLIENT FAILED");
		free(buffer);
		free_message(msg);
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
