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
#include <pthread.h>

#include "message.h"
#include "table_skel.h"
#include "network_server.h"
#include "read_write.h"
#include "message-private.h"

static volatile int keep_looping = 1;
int stop_threads = 0;

pthread_mutex_t t_mutex_write;
pthread_mutex_t t_mutex_stop;

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

void *thread_main(void* arg){
	//se nao funcionar mete atoi
	int client_socket =  *((int*) arg);
	int send_msg_result = 0, msg_process_result = 0;
	char cmessage;
	int result;
	pthread_mutex_lock(&t_mutex_stop);
	int stop_while = stop_threads;
	pthread_mutex_unlock(&t_mutex_stop);

	while((result = recv(client_socket, &cmessage, sizeof(char), MSG_PEEK)) != -1 && !stop_while ) {
		// printf("Waiting for a command...\n");
		struct message_t* message = NULL;
		// reads message from client
		if ((message = network_receive(client_socket)) != NULL) {
			// printf("New message received from the client...\n");
			// print_message(message); // for debugging purposes
			// invoke will update message, returns -1 if something fails
			msg_process_result = invoke(message);
			if (msg_process_result == -1) {
				// printf("There was an error while processing the current message\n");
				build_error_message(message);
			}
			// printf("Message that is going to be sent to the client:\n");
			// print_message(message);
			if ((send_msg_result = network_send(client_socket, message)) != -1) {
				// printf("Message was successfuly processed and sent to client\n");
			} else {
				// printf("There was an error while sending this message to the client.\n");
			}
			free_message(message);
			// printf("Message freed and ready to receive next command\n");
		}

		pthread_mutex_lock(&t_mutex_stop);
		stop_while = stop_threads;
		pthread_mutex_unlock(&t_mutex_stop);
	}
	// se saiu do while entao tem de fechar a thread
	network_server_close(client_socket);
	pthread_exit(NULL);
}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){
	pthread_t pthread;
	pthread_attr_t pthread_attr;
	socklen_t size_client = sizeof(struct sockaddr);
	int new_socket_fd = 0;
	pthread_mutex_init(&t_mutex_write, NULL);
	pthread_mutex_init(&t_mutex_stop, NULL);
	struct sockaddr_in client_address;
	if  (listening_socket < 0) {
		return -1;
	}

	/* armar o sinal para apanhar o ctrl c */
	signal(SIGINT, sigint_handler);
	
	while (keep_looping) {
		/* Create pthread argument for each connection to client. */
		/* malloc'ing before accepting a connection causes only one small
		 * memory when the program exits. It can be safely ignored.
		 */

		/* Accept connection to client. */
		new_socket_fd = accept(listening_socket, (struct sockaddr *) &client_address, &size_client);
		if (new_socket_fd == -1) {
		    perror("accept");
		    return -1;
		}

		/* Initialise pthread argument. */
		if (pthread_attr_init(&pthread_attr) != 0) {
		    perror("pthread_attr_init");
		    exit(1);
		}
		if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED) != 0) {
	            perror("pthread_attr_setdetachstate");
		    exit(1);
		}

		/* Create thread to serve connection to client. */
		if (pthread_create(&pthread, &pthread_attr, thread_main, (void*)&new_socket_fd) != 0) {
		    perror("pthread_create");
		    return -1;
		}

	}
	printf("Saiu do loop\n");

	pthread_mutex_lock(&t_mutex_stop);
	stop_threads = 1;
	pthread_mutex_unlock(&t_mutex_stop);
	// deixar as threads fechar
	sleep(5);
	if(pthread_detach(pthread) != 0){
		perror("Pthread join error");
	}

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
