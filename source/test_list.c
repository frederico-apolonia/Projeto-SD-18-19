#define NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "data.h"
#include "entry.h"
#include "list.h"

/**************************************************************/
int testListaVazia() {
	struct list_t *list = list_create();

	int result = list != NULL && list_size(list) == 0;

	list_destroy(list);
	
	printf("list - testListVazia: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testAddCauda() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *entry = entry_create(strdup("abc"),data_create(5));
	memcpy(entry->value->data,"abc1",5);

	list_add(list,entry);

	result = list_get(list,"abc") == entry && 
                 list_size(list) == 1;

	list_destroy(list);

	printf("list - testAddCauda: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testAddVarios() {
	int result,i,keysize;
	struct list_t *list = list_create();
	struct entry_t *entry[1024];
	char key[16];
	

	for(i=0; i<1024; i++) {
		sprintf(key,"keyabc-%d",i);
		keysize = strlen(key)+1;
		entry[i] = entry_create(strdup(key),data_create(keysize));
		memcpy(entry[i]->value->data,key,keysize);

		list_add(list,entry[i]);
	}

	assert(list_size(list) == 1024);
	result = (list_size(list) == 1024);

	for(i=0; i<1024; i++) {
		assert(list_get(list,entry[i]->key) == entry[i]);
		result = result && (list_get(list,entry[i]->key) == entry[i]);
	}

	list_destroy(list);

	printf("list - testAddVarios: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testRemoveCabeca() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                   *e2 = entry_create(strdup("def"),data_create(5)),
                   *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list,"abc") == 0 && 
             list_get(list,"def") == e2 && 
             list_get(list,"ghi") == e3 && 
             list_size(list) == 2;

	list_destroy(list);

	printf("list - testRemoveCabeca: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testRemoveCauda() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                   *e2 = entry_create(strdup("def"),data_create(5)),
                   *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list,"ghi") == 0 && 
             list_get(list,"abc") == e1 && 
             list_get(list,"def") == e2 && 
             list_size(list) == 2;

	list_destroy(list);

	printf("list - testRemoveCauda: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testRemoveMeio() {
	int result;
	struct list_t *list = list_create();

	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                   *e2 = entry_create(strdup("def"),data_create(5)),
                   *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list,"def") == 0 && 
             list_get(list,"abc") == e1 && 
             list_get(list,"ghi") == e3 && 
             list_size(list) == 2;

	list_destroy(list);

	printf("list - testRemoveMeio: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int testGetKeys() {
	int result;
	struct list_t *list = list_create();
	char **keys;
	struct entry_t *e1 = entry_create(strdup("abc"),data_create(5)),
                   *e2 = entry_create(strdup("def"),data_create(5)),
                   *e3 = entry_create(strdup("ghi"),data_create(5));
	memcpy(e1->value->data,"abc1",5);
	memcpy(e2->value->data,"def1",5);
	memcpy(e3->value->data,"ghi1",5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	keys = list_get_keys(list);

	result = strcmp(keys[0],e1->key) == 0 && keys[0] != e1->key &&
             strcmp(keys[1],e2->key) == 0 && keys[1] != e2->key && 
             strcmp(keys[2],e3->key) == 0 && keys[2] != e3->key && 
             keys[3] == NULL;

	list_free_keys(keys);

	list_destroy(list);

	printf("list - testGetKeys: %s\n",result?"passou":"não passou");
	return result;
}

/**************************************************************/

int main() {
	int score = 0;

	printf("iniciando teste list bin\n");

	score += testListaVazia();

	score += testAddCauda();

	score += testAddVarios();

	score += testRemoveCabeca();

	score += testRemoveCauda();

	score += testRemoveMeio();

	score += testGetKeys();

	printf("teste list bin: %d/7\n",score);

	return 0;
}
