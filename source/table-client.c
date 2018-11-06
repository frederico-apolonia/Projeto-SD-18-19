#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "data.h"
#include "entry.h"
#include "network_client.h"
#include "client_stub.h"
#include "client_stub-private.h"

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

int main(int argc, char **argv){
	struct rtable_t *rtable;
	char* splitter = " ";
	char input_utilizador[MAX_MSG];
	struct data_t * data_result;
	struct entry_t * entry_result;

	/* Testar os argumentos de entrada */
	if (argc < 2){
		printf("Exemplo de uso: ./table_client 10.101.148.144:54321\n");
		return -1;
	}	
	
	/* Iniciar instância do stub e Usar rtable_connect para estabelcer ligação ao servidor*/
	rtable = rtables_connect(argv[1]);
	if (rtable == NULL){
		perror("Erro ao estabelecer ligacao com o servidor");
		return -1;
	}

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */
 	while (1 == 1){

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		   Sugestão: usar fgets de stdio.h
		   Quando pressionamos enter para finalizar a entrada no
		   comando fgets, o carater \n é incluido antes do \0.
		   Convém retirar o \n substituindo-o por \0.
		*/
		fgets(input_utilizador, MAX_MSG, stdin);
		//substituir \n por \0
		input_utilizador[strcspn(input_utilizador, "\n")] = 0;

		char* string_op_user = strtok(input_utilizador, splitter);
		int op_user = command_to_code(string_op_user);
		// quit command
		if(op_user == 0) {
			break;
		}
		char* parametro_comando;
		char* parametro_comando_2;
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
				//vai ler o segundo valor inserido pelo utilizador no stdin
				parametro_comando = strtok(NULL, splitter);
				printf("%s\n", parametro_comando);
				if (parametro_comando == NULL){
					perror("O parametro inserido eh invalido");	
					break;
				}
				if (rtable_del(rtable, parametro_comando) == 0){
						printf("O elemento %s foi bem eliminado\n", parametro_comando);
				}else{
						printf("O elemento nao foi eliminado (key not found ou problemas)\n");
				}
				break;	
			case OP_GET:
				//input esperado pelo utilizador para esta operacao:Ex: "get <key>"
				//vai ler o segundo valor inserido pelo utilizador no stdin
				parametro_comando = strtok(NULL, splitter);
				if (parametro_comando == NULL){
					perror("O parametro inserido eh invalido");	
					break;
				}

				data_result = rtable_get(rtable,parametro_comando);
				if (data_result == NULL){
					printf("Erro ao obter o elemento com a key %s\n", parametro_comando);
				}else{
					printf("O elemento foi obtido com sucesso\n");
					print_data_struct(data_result);
				}
				data_destroy(data_result);
				break;
			case OP_PUT:
				//input esperado pelo utilizador para esta operacao: Ex: "put <key> <data>" (OP_CODE,key,elemento_para_data_t)
				//vai ler o segundo valor inserido pelo utilizador no stdin
				parametro_comando = strdup(strtok(NULL, splitter));
				//vai ler o terceiro valor inserido pelo utilizador no stdin
				parametro_comando_2 = strdup(strtok(NULL, splitter));
				//Verificar se os parametros inseridos sao != NULL
				if (parametro_comando == NULL || parametro_comando_2 == NULL){
					perror("O(s) parametro(s) inserido(s) eh invalido");	
					break;					
				}
		
				data_result = data_create2(strlen(parametro_comando_2)+1, strdup(parametro_comando_2));
				if (data_result == NULL){
					perror("Erro ao criar data_t");
					break;
				}
				entry_result = entry_create(parametro_comando,data_result);
				if (entry_result == NULL){
					data_destroy(data_result);
					perror("Erro ao criar entry_t");
					break;						
				}
				if (rtable_put(rtable,entry_result) == 0){
					printf("O elemento %s foi inserido com sucesso\n", parametro_comando);						
				}else{
					printf("Problemas ao inserir o elemento %s\n",parametro_comando);
				}
				entry_destroy(entry_result);
				break;
			case OP_GETKEYS:
				listgetKeys = rtable_get_keys(rtable);
				for (count = 0; listgetKeys[count] != NULL ; count++){
				    if(count == 0) {
				        printf("%s", listgetKeys[count]);
				    }
				    printf(", %s",listgetKeys[count]);    
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
  	return rtables_disconnect(rtable);
}

