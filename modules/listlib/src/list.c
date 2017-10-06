/*
 * list.c
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
 */

#include "listlib/list.h"
#include <string.h>

ArrayList* array_list_init(size_t initial_size, size_t step){
	ArrayList* list = (ArrayList *)malloc(sizeof(ArrayList));

	if(list != NULL){
		list->data = malloc(initial_size * step);

		if(list->data == NULL){
			return NULL;
		}

		list->max_size = initial_size;
		list->size = 0;
		list->step = step;
	}

	return list;
}

void* array_list_value_at(ArrayList* list, int32_t pos){

	if(pos < 0 || pos > list->size){
		return NULL;
	}

	size_t p = pos * list->step;
	return (list->data + p);
}

int32_t array_list_grow(ArrayList* list){

	int32_t newmax = list->max_size*2;

	void* newdata = realloc(list->data, newmax * list->step);

	if(newdata == NULL){
		return -1;
	}

	list->data = newdata;
	list->max_size = newmax;

	return 1;
}


ArrayList* array_list_delete(ArrayList* list){
	if(list != NULL){
		if(list->data != NULL){
			free(list->data);
		}

		free(list);
		list = NULL;
	}

	return NULL;
}

int32_t array_list_add_at(ArrayList* list, void* data, int32_t pos){
	if(pos == list->max_size){
		int32_t r = array_list_grow(list);
		if(r < 0){
			return -1;
		}
	}
	size_t p = pos * list->step;
	void* np = list->data + p;
	memcpy(np, data, list->step);
	return 1;
}

