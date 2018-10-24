/*
	Programa cliente para manipular tabela de hash remota.

	Uso: table-client <ip servidor>:<porta servidor>
	Exemplo de uso: ./table_client 10.101.148.144:54321
*/

#include "network_client-private.h"

int main(int argc, char **argv){
	struct rtable_t *rtable;

	/* Testar os argumentos de entrada */
	
	/* Iniciar instância do stub e Usar rtable_connect para estabelcer ligação ao servidor*/

	rtable = rtable_connect(/* */);

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */
 	while (/* condição */){

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		   Sugestão: usar fgets de stdio.h
		   Quando pressionamos enter para finalizar a entrada no
		   comando fgets, o carater \n é incluido antes do \0.
		   Convém retirar o \n substituindo-o por \0.
		*/

		/* Verificar se o comando foi "quit". Em caso afirmativo
		   não há mais nada a fazer a não ser terminar decentemente.
		*/
		
		/* Caso contrário:
			Verificar qual o comando;	
			chamar função do stub para processar o comando e receber msg_resposta
		*/

	}
  	return rtable_disconnect(rtable);
}

