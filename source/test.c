#include <stdlib.h>
#include "list.h"
#include "list-private.h"
#include "entry.h"
#include "data.h"

int main() {
    //Testar 1st funcao de data
    struct data_t *data_teste = data_create(sizeof(struct data_t));
    data_teste->datasize = 1234;    
    printf("Datasize: %d\n",data_teste->datasize);    
    
    int *conteudo_data = (int*)malloc(sizeof(int));
    int i = 9;
    conteudo_data = i;
    printf("Conteudo: %d\n", conteudo_data);
    
    data_teste->data = conteudo_data;
    //printf("%d",*((int)*data_teste->data)); RIP
    
    //Testar 2nd funcao de data
    struct data_t *data_teste_2nd = data_create2(sizeof (conteudo_data),10);
    printf("Conteudo dentro de data: %d\n", data_teste_2nd->data);
    
	struct data_t *dataDup = data_dup(data_teste_2nd);
    printf("Conteudo em data duplicado: %d\n", dataDup->data);
    /**
    free(data_teste_2nd->data);
    free(data_teste_2nd);
    printf("Conteudo em data duplicado depois de libertado o inicial: %d\n", dataDup->data);*/
    
    int *conteudo = 9;
    struct data_t *testData = data_create2(sizeof (conteudo),conteudo);
    printf("Conteudo dentro de testData: %d\n", testData->data);
    
    
    struct list_t *newList = list_create();
    if(newList->head ==  NULL){
    	printf("IT'S FUKIN NULL'\n");
	}
	
    struct entry_t *newEntry = entry_create("hello", data_teste);
    printf("Valor em data de Entry: %d\n", newEntry->value->data);
    printf("Valor de chave de Entry: %s\n", newEntry->key);
    struct data_t *data2 = data_create2(sizeof (conteudo_data),2);
    struct data_t *data3 = data_create2(sizeof (conteudo_data),3);
    struct data_t *data4 = data_create2(sizeof (conteudo_data),4);
    struct data_t *data5 = data_create2(sizeof (conteudo_data),5);
    struct entry_t *newEntry2 = entry_create("hello1", data2);
    struct entry_t *newEntry3 = entry_create("hello2", data3);
    struct entry_t *newEntry4 = entry_create("hello3", data4);
    struct entry_t *newEntry5 = entry_create("hello4", data5);
    list_add(newList,newEntry);
    list_add(newList,newEntry2);
    list_add(newList,newEntry3);
    list_add(newList,newEntry4);
    list_add(newList,newEntry5);
    list_print(newList);
    printf("%s\n",newList->head->data->key);
    if(strcmp(newList->head->data->key,"hello") == 0){
		printf("SAO IGUAIS ----------------------\n");
	}
    printf("REMOCAO DE NOH---------------------\n");
    list_remove(newList,"hello");
    list_print(newList);
    printf("REMOCAO DE NOH A MEIO DA LISTA---------------------\n");
    list_remove(newList,"hello4");
    list_print(newList);
    printf("FIN");
}
