/*
 * intset.c
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
 */
#include "hdbscan/intset.h"
#include <stdio.h>

IntArraySet* int_array_set_init(){
	return int_array_list_init();
}

IntArraySet* int_array_set_init_size(int32_t size){
	return int_array_list_init_size(size);
}

void int_array_set_insert(IntArraySet* list, int32_t data){

	if(int_array_set_search(list, data) < 0){
		int_array_list_append(list, data);
	}
}

int32_t int_array_set_insert_at(IntArraySet* set, int32_t data, int32_t position){

	size_t count = set->size;
	int32_t *elements;
	size_t i;

	if (count == set->size){
		extend_array(set);
	}
	elements = set->data;
	for (i = count; i > position; i--)
		elements[i] = elements[i - 1];
	elements[position] = data;
	set->size = count + 1;
	return 1;
}

int int_array_set_insert_sorted(IntArraySet* set, int32_t data){
	int32_t count = set->size;
	int32_t low = 0;

	if (count > 0) {
		int32_t high = count;

		do{

			int32_t mid = low + (high - low)/2;
			int32_t cmp = cmpint(set->data + mid, &data);

			if (cmp < 0)
				low = mid + 1;
			else if (cmp > 0)
				high = mid;
			else
				/* cmp == 0 */
				return 0;

		} while(low < high);
	}
	return int_array_set_insert_at(set, data, low);
}

int32_t int_array_set_search(IntArraySet* list, int32_t data){
	return int_array_list_search(list, data);
}

int32_t int_array_set_search_sorted(IntArraySet* list, int32_t data){
	return int_array_list_search_sorted(list, data);
}

int32_t int_array_set_remove(IntArraySet* list, int32_t data){
	return int_array_list_remove(list, data);
}

void int_array_set_delete(IntArraySet* list){
	int_array_list_delete(list);
}

int32_t int_array_set_data(IntArraySet* list, int32_t idx){
	return int_array_list_data(list, idx);
}

void int_array_set_sort(IntArraySet* list){
	int_array_list_sort(list);
}
