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
	int client_socket, msg_size, result, size_to_client;
	char* buffer;
	struct message_t* message, *ans_message;

	while((client_socket = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1){
		// reads message size from client
		msg_size = ntohl(read(listening_socket, msg_size, sizeof(int)));
		if (msg_size > 0) {
			if ((buffer = malloc(msg_size) == NULL)) {
				// if malloc failed
				close(listening_socket);
				perror("Malloc failed");
				return -1;
			}

			if(result = read_all(client_socket, buffer, msg_size) != msg_size) {
				/* sinal de que a conexão foi fechada pelo cliente
				 * ou ficaram bytes perdidos
				 */
				close(listening_socket);
				free(buffer);
				perror("Reading client buffer failed");
				return -1;
			} else {/* processamento da requisição e da resposta */
				message = buffer_to_message(buffer, msg_size);
				if (result = invoke(message) == -1) {
					close(listening_socket);
					free(buffer);
					free_message(message);
				}
				// sends msg_size to client
				msg_size = message_to_buffer(ans_message, buffer);
				size_to_client = htonl(msg_size);
				if (result = write_all(listening_socket, size_to_client, sizeof(int) != sizeof(int))) {
					close(listening_socket);
					free(buffer);
					free_message(message);
					perror("Failed while writing message size");
					return -1;
				}

			}
		}
		
		if (socket_de_cliente com erro) {
			close(socket_de_cliente);
		}
	}

	
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
	
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
	
}

/* A função network_server_close() fecha a ligação estabelecida por
 * network_server_init().
 */
int network_server_close(){

}
