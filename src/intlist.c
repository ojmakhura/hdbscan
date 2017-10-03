/*
 * intlist.c
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
 */
#include <math.h>
#include <string.h>
#include "hdbscan/intlist.h"


int32_t cmpint(const void * ptr_a, const void * ptr_b) {

	int32_t a, b;
	a = *(int32_t *) ptr_a;
	b = *(int32_t *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

int32_t highestPowerof2(int32_t n)
{
	int32_t p = (int32_t)log2(n);
   return (int32_t)pow(2, p);
}

IntList* int_list_init(){
	IntList* list = (IntList*)malloc(sizeof(IntList));

	if(list != NULL){
		list->size = 0;
		list->head = NULL;
		list->tail = NULL;
	}

	return list;
}

node* int_list_add(IntList* list, int32_t data){
	struct Node*  nd = (struct Node* )malloc(sizeof(node));
	nd->next = NULL;
	nd->prev = NULL;

	if(nd != NULL){
		nd->data = data;

		if(list->size == 0){
			list->head = nd;
		} else{
			list->tail->next = nd;
			nd->prev = list->tail;
		}
		list->tail = nd;
		list->size ++;

	} else{
		return NULL;
	}

	return nd;
}

node* int_list_remove(IntList* list, int data){

	node* nd = int_list_search(list, data);

	if(nd != NULL){

		if(list->head != nd && list->head != nd){
			nd->prev->next = nd->next;
			nd->next->prev = nd->prev;
			nd->prev = NULL;
			nd->next = NULL;

		}else{
			if(list->head == nd){
				list->head = nd->next;
				if(nd->next != NULL){
					nd->next = NULL;
				}
			}

			if(list->tail == nd){
				list->tail = nd->next;
				if(nd->prev != NULL){
					nd->prev = NULL;
				}
			}
		}

		list->size --;
	}

	return nd;

}
void int_list_remove_full(IntList* list, int32_t data){

	node* nd = int_list_remove(list, data);

	if(nd != NULL){
		free(nd);
	}

}
node* int_list_search(IntList* list, int32_t data){

	node* nd = list->head;
	while(nd != NULL){

		if(nd->data == data){
			break;
		}
		nd = nd->next;
	}
	return nd;
}


void int_list_delete(IntList* list){

	node* nd = list->head;
	while(nd != NULL){
		node* tmp = nd;

		nd = nd->next;
		nd->prev = NULL;
		free(tmp);
	}

	if(list != NULL){
		free(list);
		list = NULL;
	}
}

/****************************************************************************************
 * Implementation of array lists
 ****************************************************************************************/


IntArrayList* int_array_list_init(){

	return int_array_list_init_size(256);
}

IntArrayList* int_array_list_init_size(int32_t size){

	IntArrayList* list = (IntArrayList*) malloc(sizeof(IntArrayList));

	if(list != NULL){
		list->max_size = highestPowerof2(size*2);
		list->data = (int32_t*)malloc(list->max_size * sizeof(int32_t));
		list->size = 0;
	}

	return list;
}

int32_t list_extend_array(IntArrayList* list){

	int32_t newmax = list->max_size*2;

	int* newdata = realloc(list->data, newmax * sizeof(int32_t));

	if(newdata == NULL){
		return -1;
	}

	list->data = newdata;
	list->max_size = newmax;

	return 1;
}

void int_array_list_append(IntArrayList* list, int32_t data){

	if(list->size == list->max_size){
		list_extend_array(list);
	}
	list->data[list->size] = data;
	list->size++;
}


void int_array_list_pop(IntArrayList* list){
	list->size--;
}

int32_t int_array_list_search(IntArrayList* list, int32_t data){
	int32_t idx = -1;

	for(int i = 0; i < list->size; i++){
		if(list->data[i] == data){
			idx = i;
			break;
		}
	}

	return idx;
}


int32_t int_array_list_search_sorted(IntArrayList* list, int32_t data){

	int32_t idx = -1;
	int32_t* dptr = bsearch(&data, list->data, sizeof(int32_t), list->size, cmpint);

	if(dptr != NULL){
		idx = dptr - list->data;
	}

	return idx;
}

int32_t int_array_list_remove(IntArrayList* list, int32_t data){
	int32_t rep = -1;

	int32_t idx = int_array_list_search(list, data);

	if(idx != -1){
		int32_t* to = list->data + idx;
		int32_t* from = list->data + idx +1;
		int32_t cpysize = list->size - idx -1;

		memcpy(to, from, cpysize * sizeof(int32_t));

		list->size--;
	}

	return rep;
}

void int_array_list_insert_at(IntArrayList* list, int32_t data, int32_t index){
	if(list->size == list->max_size){
		extend_array(list);
	}

	int32_t* to = list->data + index + 1;
	int32_t* from = list->data + index;
	int32_t cpysize = list->size - index;

	memcpy(to, from, cpysize * sizeof(int32_t));
	list->size++;

}

void int_array_list_extend(IntArrayList* dest, IntArrayList* src){
	int32_t cmbsize = dest->size + src->size;
	
	if(cmbsize >= dest->size){
		list_extend_array(dest);
	}
	
	if(dest->data != NULL){
		int* to = dest->data + dest->size;
		memcpy(to, src->data, (src->size)*sizeof(int));
	}

}

void int_array_list_delete(IntArrayList* list){
	if(list != NULL){
		if(list->data != NULL){
			free(list->data);
		}

		free(list);
		list = NULL;
	}
}

int32_t int_array_list_data(IntArrayList* list, int32_t idx){
	return (list->data)[idx];
}


void int_array_list_sort(IntArrayList* list){
	qsort(list->data, list->size, sizeof(int32_t), cmpint);
}

