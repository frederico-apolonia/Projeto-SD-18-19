#include <stdlib.h>
#include "list.h"
#include "list-private.h"
#include "entry.h"
#include "data.h"
#include "table.h"
#include "table-private.h"

int main() {
    //Testar 1st funcao de data
    struct data_t *data_teste = data_create(sizeof(struct data_t));//
    printf("Datasize: %d\n",data_teste->datasize);

    char *conteudo_data = (char*)malloc(sizeof(char)+1);//
    strcpy(conteudo_data, "9");

    struct data_t *new_data_test = data_create2(sizeof(conteudo_data), conteudo_data);//
    printf("Conteudo: %s\n", new_data_test->data);

    data_teste->data = conteudo_data;

    //Testar 2nd funcao de data
    struct data_t *data_teste_2nd = data_create2(sizeof (conteudo_data),conteudo_data);//
    printf("TEST=============================\n");
    printf("Conteudo dentro de data: %s\n", data_teste_2nd->data);

	printf("START DUP\n");
    struct data_t *dataDup = data_dup(data_teste_2nd); //
    printf("Conteudo em data duplicado: %s\n", dataDup->data);

    data_destroy(data_teste_2nd );
    printf("Conteudo em data duplicado depois de libertado o inicial: %s\n", dataDup->data);

    char *conteudo = (char*)malloc(sizeof(char)+1);
    strcpy(conteudo, "9");
    struct data_t *testData = data_create2(sizeof (conteudo),conteudo); //
    printf("Conteudo dentro de testData: %d\n", testData->data);


    struct list_t *newList = list_create(); //
    if(newList->head ==  NULL){
    	printf("IT'S FUKIN NULL'\n");
	  }

    char *conteudo2 = (char*)malloc(sizeof(char)+1);
    strcpy(conteudo2, "a");
    char *conteudo3 = (char*)malloc(sizeof(char)+1);
    strcpy(conteudo3, "b");
	  char *conteudo4 = (char*)malloc(sizeof(char)+1);
    strcpy(conteudo4, "c");
    char *conteudo5 = (char*)malloc(sizeof(char)+1);
    strcpy(conteudo5, "d");

    char *key2 = (char*)malloc(sizeof(char)+1);
    strcpy(key2, "1");
    char *key3 = (char*)malloc(sizeof(char)+1);
    strcpy(key3, "2");
    char *key4 = (char*)malloc(sizeof(char)+1);
    strcpy(key4, "3");
    char *key5 = (char*)malloc(sizeof(char)+1);
    strcpy(key5, "4");
    struct entry_t *newEntry = entry_create(conteudo, data_teste);
    printf("Valor em data de Entry: %d\n", newEntry->value->data);
    printf("Valor de chave de Entry: %s\n", newEntry->key);
    struct data_t *data2 = data_create2(sizeof (conteudo_data),conteudo2);
    struct data_t *data3 = data_create2(sizeof (conteudo_data),conteudo3);
    struct data_t *data4 = data_create2(sizeof (conteudo_data),conteudo4);
    struct data_t *data5 = data_create2(sizeof (conteudo_data),conteudo5);
    //para o free funcionar bem, o key de cada entry tmb tem q ter um malloc
    //reutiliza-se os conteudos anteriores para servirem de key
    struct entry_t *newEntry2 = entry_create(key2, data2);
    struct entry_t *newEntry3 = entry_create(key3, data3);
    struct entry_t *newEntry4 = entry_create(key4, data4);
    struct entry_t *newEntry5 = entry_create(key5, data5);
    list_add(newList,newEntry);
    list_add(newList,newEntry2);
    list_add(newList,newEntry3);
    list_add(newList,newEntry4);
    list_add(newList,newEntry5);
    list_print(newList);
    printf("Tamanho da lista: %d\n",newList->size);
    printf("%s\n",newList->head->data->key);

    printf("REMOCAO DE NOH---------------------\n\n");
    list_remove(newList,conteudo3);
    list_print(newList);
    printf("Tamanho da lista: %d\n",newList->size);
    printf("REMOCAO DE NOH NAO EXISTENTE---------------------\n\n");
    list_remove(newList,"hello4");
    printf("Tamanho da lista: %d\n",newList->size);
    list_print(newList);
    printf("REMOCAO DE NOH A MEIO DA LISTA---------------------\n\n");
    list_remove(newList,conteudo4);
    printf("Tamanho da lista: %d\n",newList->size);
    list_print(newList);


    char** keys = list_get_keys(newList);
	  printf("%d\n",sizeof(NULL));
    printf("LIBERTACAO DE LIST KEYS\n");

    struct entry_t *getEntry = list_get(newList,conteudo4);//
    printf("TEST\n");
    if(getEntry == NULL){
      printf("NULL\n");
    }else{
      printf("===================================================== %s\n",getEntry->value->data);
    }

    struct table_t *tabela = table_create(2);
    table_put(tabela,key3,data3);
    table_put(tabela,key5,data5);
    table_put(tabela,key2,data2);
    table_put(tabela,key4,data4);
    table_put(tabela,conteudo,data_teste);
    table_print(tabela);
    struct data_t *temp = table_get(tabela,key3);
    printf("%s\n",temp->data);

    table_del(tabela,key3);
    table_print(tabela);
    printf("%d\n",table_size(tabela));

    char **tablekeys = table_get_keys(tabela);
    int ind;
    for(ind = 0; tablekeys[ind] != NULL; ind++){
      printf("%s\n", tablekeys[ind]);
    }


    data_destroy(data_teste);
    free(conteudo_data);
    data_destroy(new_data_test);
    data_destroy(data_dup);
    //free(conteudo);
    data_destroy(testData);
    list_destroy(newList);
    /*free(conteudo2);
    free(conteudo3);
    free(conteudo4);
    free(conteudo5);
    free(key2);
    free(key3);
    free(key4);
    free(key5);
    entry_destroy(newEntry);
    entry_destroy(newEntry2);
    entry_destroy(newEntry3);
    entry_destroy(newEntry4);
    entry_destroy(newEntry5);*/
    list_free_keys(keys);
    entry_destroy(getEntry);
    table_destroy(tabela);
    data_destroy(temp);
    table_free_keys(tablekeys);
    printf("FIN\n");
}
