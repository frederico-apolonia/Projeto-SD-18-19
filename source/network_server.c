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


	while((socket_de_cliente = accept(listening_socket) != -1)){
		if (socket_de_cliente tem dados para ler) {
			nbytes = read_all(socket_de_cliente, buffer, …);
			if(read returns 0 bytes) {
				/* sinal de que a conexão foi fechada pelo cliente */
				close(socket_de_cliente);
			} else {/* processamento da requisição e da resposta */
				message = buffer_to_message(buffer);
				msg_out = invoke(message);
				buffer = message_to_buffer(msg_out);
				write_all(socket_de_cliente, buffer, …);
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
