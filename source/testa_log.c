#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include "persistence_manager.h"
#include "persistence_manager-private.h"
#include "table.h"
#include "entry.h"
#include "data.h"
#include "message.h"
int main(int argc, char const *argv[]) {
  struct table_t *tabelinha =  table_create(2);
  struct pmanager_t* managerzinho = pmanager_create("alibaba",256);
  struct message_t *mensagemzinha = (struct message_t*) malloc (sizeof(struct message_t));
  mensagemzinha->opcode = 40;
  mensagemzinha->c_type = 30;
  struct data_t* datazinha = data_create2(4, strdup("abcd"));
  mensagemzinha->content.entry = entry_create(strdup("calcas_do_Fred"), datazinha);
  int result = 0;
  printf("bom dia.\n");
  result = pmanager_log(managerzinho, mensagemzinha);

  printf("RESULT VALUE: %d :)\n", result);
  
  //Verifica se o log tem dados
  int result2 = pmanager_have_data(managerzinho);
  if (result2 == 1){
	  printf("Existem dados\n");
  }else{
	   printf("ERRO\n");
  }
  
  //Verificar a copia da tabela para o ficheiro (WORKING)
  int result3 = table_put(tabelinha,"teste1st_data",datazinha);
  printf("%d\n",result3);
  struct data_t* datazona = data_create2(4, strdup("edfg"));
  result3 = table_put(tabelinha,"teste2nd_data",datazona);
  struct data_t* datazorra = data_create2(4, strdup("hijk"));
  result3 = table_put(tabelinha,"teste3rd_data",datazorra);
  //int result4 = table_write_stt(managerzinho->logfile,tabelinha);
  //printf("%d\n",result4);
  
  //Teste para o pmanager foi destruido
  int res = pmanager_destroy(managerzinho);
  printf("%d\n",res);
 
  
  //Teste para ver se o pmanager foi destruido e as files .log e .ckp foram apagados 
  /*int res2 = pmanager_destroy_clear(managerzinho);
  printf("%d\n",res2);*/
  
  // TEste da criacao do ficheiro .stt
  int res3 = pmanager_store_table(managerzinho,tabelinha);
  printf("%d\n",res3);
  
  //Teste para verificar a limpeza de .log e a copia do mesmo paar .stt e .ckp
  int res4 = pmanager_rotate_log(managerzinho);
  printf("%d\n",res4);
  
  // Teste para verificar a copia de um ficheiro para a tabela.
  struct table_t *tabelazorra =  table_create(2);
  int res5 = pmanager_fill_state (managerzinho,tabelazorra);
  printf("%d\n",res5);
  return 0;
}
