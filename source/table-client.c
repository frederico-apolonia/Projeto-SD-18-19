
/*  Grupo 034
 *  Francisco Grilo - 49497
 *  Frederico Apolónia - 47892
 *  Ye Yang - 49521
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#include "message.h"
#include "data.h"
#include "entry.h"
#include "network_client.h"
#include "client_stub.h"
#include "client_stub-private.h"

int FIRST_TRY = 10;
int SLEEP_FINISHED = 0;
pthread_mutex_t t_mutex;
pthread_mutex_t t_result_mutex;
pthread_mutex_t t_ftry_mutex;
pthread_mutex_t t_finish_mutex;
struct threads_result *thr_result;
struct thread_params *thr_params;
/*
	Programa cliente para manipular tabela de hash remota.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/
int command_to_code(char* command) {
	if(strcmp(command, "size") == 0) {
		return OP_SIZE;
	}
	else if (strcmp(command, "del") == 0) {
		return OP_DEL;
	}
	else if (strcmp(command, "get") == 0) {
		return OP_GET;
	}
	else if (strcmp(command, "put") == 0) {
		return OP_PUT;
	}
	else if (strcmp(command, "getkeys") == 0) {
		return OP_GETKEYS;
	}
	else if (strcmp(command, "quit") == 0) {
		return 0;
	}
	else {
		return -1;
	}
}

/* Imprime os elementos de uma data_struct */
void print_data_struct(struct data_t* data) {
	printf("Datasize: %d\n", data->datasize);
	printf("Data: %s\n", (char*) data->data);
}

//Torna um inteiro numa string
void int_to_str(int src, char** dest){
	int key_size = (int) ((ceil(log10(src))+1)*sizeof(char));
	*dest = malloc(key_size);
	sprintf(*dest, "%d", src);
}

void *thread_main(void *arg) {
	struct rtable_t *rtable;
	int num_puts_gets = 0, op = thr_params->op;
	int num_key;
	struct entry_t *entry;
	struct data_t *data, *temp_data;
	int value = getpid();
	char* value_str, *key_str;
	double time_spent = 0.0;
	clock_t begin,end;
	/* Conectar ao servidor */
	rtable = rtables_connect(thr_params->address_port);
	pthread_mutex_lock(&t_finish_mutex);
	int stop_while = SLEEP_FINISHED;
	pthread_mutex_unlock(&t_finish_mutex);

	
	do {
		// obter chave partilhada
		pthread_mutex_lock(&t_mutex);
		thr_params->key++;
		num_key = (int) thr_params->key;
		pthread_mutex_unlock(&t_mutex);

		// tamanho da chave para colocar em string
		int_to_str(num_key, &key_str);
		switch (op) {
			case OP_PUT:
				int_to_str(value, &value_str);
				data = data_create2(strlen(value_str),value_str);
				entry = entry_create(key_str, data);
				begin = clock();
				if (rtable_put(rtable, entry) == -1) {
					continue;
				}
				end = clock();
				time_spent += (double) (end - begin)/CLOCKS_PER_SEC;
				break;

			case OP_GET:
				begin = clock();
				temp_data = rtable_get(rtable, key_str);
				if(temp_data == NULL) {
					continue;
				}
				end = clock();
				time_spent += (double) (end - begin)/CLOCKS_PER_SEC;
				data_destroy(temp_data);
				break;

			default:
				break;
		}
		num_puts_gets++;

		pthread_mutex_lock(&t_finish_mutex);
		stop_while = SLEEP_FINISHED;
		pthread_mutex_unlock(&t_finish_mutex);
	} while (!stop_while);
	// guardar resultados na estrutura result
	// printf("Saiu do ciclo\n");
	pthread_mutex_lock(&t_result_mutex);
	// printf("DEBUG: Valor GLOBAL put/get %d\n", thr_result->num_put_get);
	thr_result->num_put_get += num_puts_gets;
	thr_result->time += time_spent;
	// printf("DEBUG: Valor GLOBAL put/get apos soma %d\n", thr_result->num_put_get);
	pthread_mutex_unlock(&t_result_mutex);
	rtables_disconnect(rtable);
	// printf("INFO: Thread terminou execução\n");
	pthread_exit(NULL);

}

int main(int argc, char **argv){

	char* addr_port = (char*) malloc(strlen(argv[1]));
	strcpy(addr_port, argv[1]);

	/* Ignore SIGPIPE */
	signal(SIGPIPE, SIG_IGN);

	/* Testar os argumentos de entrada */
	if (argc < 2){
		printf("Exemplo de uso table interativo: ./table_client 10.101.148.144:54321\n");
		return -1;
	}

	/* Modo interativo */
	if(argc == 2) {
		struct rtable_t *rtable;
		char* splitter = " ";
		char input_utilizador[MAX_MSG];
		struct data_t * data_result = NULL;
		struct entry_t * entry_result = NULL;

		/* Iniciar instância do stub e Usar rtable_connect para estabelcer ligação ao servidor*/
		rtable = rtables_connect(addr_port);
		if (rtable == NULL){
			perror("Erro ao estabelecer ligacao com o servidor");
			return -1;
		}

		/* Fazer ciclo até que o utilizador resolva fazer "quit" */
		while (1 == 1){

			if(FIRST_TRY < 0) {
				break;
			}
			printf(">>> "); // Mostrar a prompt para inserção de comando

			/* Receber o comando introduzido pelo utilizador
			Sugestão: usar fgets de stdio.h
			Quando pressionamos enter para finalizar a entrada no
			comando fgets, o carater \n é incluido antes do \0.
			Convém retirar o \n substituindo-o por \0.
			*/
			int num_params;
			char** input_tokens;
			fgets(input_utilizador, MAX_MSG, stdin);
			//substituir \n por \0
			input_utilizador[strcspn(input_utilizador, "\n")] = 0;
			input_tokens = tokenizer(input_utilizador, splitter, &num_params);
			int op_user = command_to_code(input_tokens[0]);
			// quit command
			if(op_user == 0) {
				break;
			}

			int size = 0;

			char** listgetKeys;
			int count;

			switch(op_user){
				case OP_SIZE:
					//input esperado pelo utilizador para esta operacao:Ex: "size"
					size = rtable_size(rtable);
					printf("Table size: %d\n",size);
					break;
				case OP_DEL:
					//input esperado pelo utilizador para esta operacao:Ex: "del <key>"
					//vai comparar o numero de argumentos inseridos  pelo utilizador
					if(num_params != 2) {
						printf("ERROR: Input must be like:\n");
						printf("'del <key>'\n");
						break;
					}

					if (rtable_del(rtable, input_tokens[1]) == 0){
							printf("O elemento %s foi bem eliminado\n", input_tokens[1]);
					}else{
							printf("O elemento nao foi eliminado (key not found ou problemas)\n");
					}
					break;
				case OP_GET:
					//input esperado pelo utilizador para esta operacao:Ex: "get <key>"
					//vai ler o segundo valor inserido pelo utilizador no stdin
					if(num_params != 2) {
						printf("ERROR: Input must be like:\n");
						printf("'get <key>'\n");
						break;
					}

					data_result = rtable_get(rtable,input_tokens[1]);
					if (data_result == NULL){
						printf("Erro ao obter o elemento com a key %s\n", input_tokens[1]);
					}else{
						printf("O elemento foi obtido com sucesso\n");
						print_data_struct(data_result);
					}
					data_destroy(data_result);
					break;
				case OP_PUT:
					//input esperado pelo utilizador para esta operacao: Ex: "put <key> <data>" (OP_CODE,key,elemento_para_data_t)
					if(num_params != 3) {
						printf("ERROR: Input must be like:\n");
						printf("'put <key> <value>'\n");
						break;
					}

					data_result = data_create2(strlen(input_tokens[2]), strdup(input_tokens[2]));
					if (data_result == NULL){
						perror("Erro ao criar data_t");
						break;
					}
					print_data_struct(data_result);
					entry_result = entry_create(strdup(input_tokens[1]),data_result);
					if (entry_result == NULL){
						data_destroy(data_result);
						perror("Erro ao criar entry_t");
						break;
					}
					if (rtable_put(rtable,entry_result) == 0){
						printf("O elemento %s foi inserido com sucesso\n", input_tokens[1]);
					}else{
						printf("Problemas ao inserir o elemento %s\n",input_tokens[1]);
					}

					break;
				case OP_GETKEYS:
					listgetKeys = rtable_get_keys(rtable);
					for (count = 0; listgetKeys[count] != NULL ; count++){
						if(count == 0) {
							printf("%s", listgetKeys[count]);
						} else {
							printf(", %s",listgetKeys[count]);
						}
					}
					printf("\n");
					rtable_free_keys(listgetKeys);
					break;
				// quit
				default:
					printf("Comando nao valido!\n");
					break;
			}
		}
		rtables_disconnect(rtable);
	}
	/* Modo automatico, nao e necessario verificar args de entrada */
	else {
		int num_threads, secs;
		int first_key = 0;
		pthread_t thr;
		//mutex init
		pthread_mutex_init(&t_mutex, NULL);
		pthread_mutex_init(&t_result_mutex, NULL);
		pthread_mutex_init(&t_finish_mutex, NULL);
		pthread_mutex_init(&t_ftry_mutex, NULL);
		/* Inicializar os parametros da funcao da thread */
		thr_params = (struct thread_params*) malloc(sizeof(struct thread_params));
		thr_params->address_port = addr_port;
		thr_params->op = strcmp(argv[2],"p") == 0 ? OP_PUT:OP_GET;
		thr_params->key = first_key;

		/* Recolher numero de segundos e de threads */
		secs = atoi(argv[3]);
		num_threads = atoi(argv[4]);

		printf("/***********************************************/\n");
		printf("Numero de segundos: %d Numero de threads: %d\n", secs, num_threads);
		printf("Comando selecionado: %s\n", strcmp(argv[2],"p") == 0 ? "put":"get");
		printf("/***********************************************/\n");
		
		/* Inicializar estrutura do result */
		thr_result = (struct threads_result*) malloc(sizeof(struct threads_result));
		thr_result->num_put_get = 0;
		thr_result->time = 0.0;

		// lançar N threads
		while (num_threads > 0) {
			if(pthread_create(&thr, NULL, &thread_main, NULL) != 0) {
				perror("\nThread não criada.\n");
				exit(EXIT_FAILURE);
			}
			num_threads--;
		}

		/* Deixar as threads trabalhar enquando o processo principal dorme */
		sleep(secs);
		pthread_mutex_lock(&t_finish_mutex);
		SLEEP_FINISHED = 1;
		pthread_mutex_unlock(&t_finish_mutex);
		// deixar as threads fechar
		printf("Execução automática terminou.\n");
		sleep(8);

		/* Junçao das threads */
		if(pthread_detach(thr) != 0){
			perror("Pthread join error");
		}

		printf("/***********************************************/\n");
		printf("Resultados finais:\n");
		printf("Numero de segundos: %d Numero de threads: %s\n", secs, argv[4]);
		printf("Comando executado: %s\n", strcmp(argv[2],"p") == 0 ? "put":"get");
		pthread_mutex_lock(&t_result_mutex);
		printf("Numero de operacoes: %d\n", thr_result->num_put_get);
		double avg_latency = thr_result->time / thr_result->num_put_get;
		pthread_mutex_unlock(&t_result_mutex);
		printf("Latencia media: %f\n", avg_latency);

		free(thr_result);
		thr_params->address_port = NULL;
		free(thr_params);

		// destroy mutexes
		pthread_mutex_destroy(&t_mutex);
		pthread_mutex_destroy(&t_result_mutex);
		pthread_mutex_destroy(&t_ftry_mutex);
		pthread_mutex_destroy(&t_finish_mutex);
	}
}
