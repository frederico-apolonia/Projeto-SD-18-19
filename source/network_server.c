#include "message.h"
#include "table_skel.h"
#include "network_server.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar 0 (OK) ou -1 (erro).
 */
int network_server_init(short port){
	int sockfd;
	struct sockaddr_in server;

	// creates TCP socket
	if (sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
		perror("Error while creating socket");
		return -1;
	}

	// fills struct server with addresses to bind to the socket
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port)); // TCP Port
	server.sin_addr.s_addr = htonl(INADDR_ANY); // All machine addresses

	//bind
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Error while binding socket");
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
	int client_socket, result;
	struct message_t* message;

	while((client_socket = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1){
		// reads message from client
		if (message = network_receive(client_socket) != NULL) {
			// invoke will update message, returns -1 if something fails
			if (result = invoke(message) == -1) {
				free_message(message);
			} else {
				if (result = network_send(client_socket, message) != -1) {
					printf("Message was successfuly processed and sent to client");
				}
			}
		}
		// close connection with client
		close(client_socket);
	}	
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
	struct message_t *message = NULL;
	int result, msg_size;
	char *buffer;

	msg_size = ntohl(read(client_socket, msg_size, sizeof(int)));
	if (msg_size > 0) {
		if ((buffer = malloc(msg_size) == NULL)) {
			// if malloc failed
			perror("Malloc failed");
			return NULL;
		}

		if(result = read_all(client_socket, buffer, msg_size) != msg_size) {
			/* sinal de que a conexão foi fechada pelo cliente
			* ou ficaram bytes perdidos
			*/
			free(buffer);
			perror("Reading client buffer failed");
			return NULL;
		} else {/* processamento da requisição e da resposta */
			message = buffer_to_message(buffer, msg_size);
			free(buffer);
			return message;
		}
	}
	return NULL;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg) {
	int msg_size, size_to_client, function_result;
	char** buffer = NULL;

	if(msg_size = message_to_buffer(msg, &buffer) < 0 ) {
		perror("MESSAGE SERIALIZATION FAILED");
		free(buffer);
		return -1;
	}
	// sends message size to client
	size_to_client = htonl(msg_size);
	if (function_result = write(client_socket, size_to_client, sizeof(int) != sizeof(int))) {
		free(buffer);
		free_message(msg);
		perror("SEND MESSAGE SIZE TO CLIENT FAILED");
		return -1;
	}
	// sends message to client
	if (function_result = write(client_socket, buffer, msg_size) != msg_size) {
		perror("SEND MESSAGE TO CLIENT FAILED");
		free(buffer);
		free_message(msg);
		return -1;
	}
	free(buffer);
	free_message(msg);
	return 0;
}

/* A função network_server_close() fecha a ligação estabelecida por
 * network_server_init().
 */
int network_server_close(){

}
