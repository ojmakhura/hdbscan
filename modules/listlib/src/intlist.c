/*
 * intlist.c
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
 * 
 * Copyright 2018 Onalenna Junior Makhura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "listlib/intlist.h"
#include "listlib/utils.h"


/****************************************************************************************
 * Implementation of array lists for integers
 ****************************************************************************************/


IntArrayList* int_array_list_init(){

	return int_array_list_init_size(128);
}

IntArrayList* int_array_list_init_size(int32_t size){

	IntArrayList* list = (IntArrayList*)array_list_init( highestPowerof2(size*2), sizeof(int32_t));
	return list;
}

IntArrayList* int_array_list_init_exact_size(int32_t size)
{
	return (IntArrayList*)array_list_init(size, sizeof(int32_t));
}

IntArrayList* int_array_list_init_full(int32_t size, int32_t value){

	IntArrayList* list = int_array_list_init_size(size);
	int32_t* ldata = (int32_t *)list->data;

	for(int32_t i = 0; i < size; i++){
		ldata[i] = value;
	}
	list->size = size;
	return list;
}

void int_array_list_append(IntArrayList* list, int32_t data){

	if(list->size == list->max_size){
		array_list_grow(list);
	}
	int32_t* ldata = (int32_t *)list->data;
	ldata[list->size] = data;
	list->size++;
}


int32_t int_array_list_pop(IntArrayList* list){
	if(list->size > 0){
		list->size--;
		return 1;
	} else{
		return -1;
	}
}

int32_t int_array_list_search(IntArrayList* list, int32_t data){
	int32_t idx = -1;

	int32_t* ldata = (int32_t *)list->data;
	for(int i = 0; i < list->size; i++){
		if(ldata[i] == data){
			idx = i;
			break;
		}
	}

	return idx;
}

int32_t int_array_list_search_sorted(IntArrayList* list, int32_t data){

	int32_t idx = -1;
	int32_t* ldata = (int32_t *)list->data;
	int32_t* dptr = bsearch(&data, ldata, sizeof(int32_t), list->size, int_compare);

	if(dptr != NULL){
		idx = dptr - ldata;
	}

	return idx;
}

int32_t int_array_list_remove_at(IntArrayList* list, int32_t idx){
	if(idx < 0 || idx > list->size){
		return -1;
	}

	int32_t* ldata = (int32_t *)list->data;
	for(int32_t i = idx + 1; i < list->size; i++){
		ldata[i-1] = ldata[i];
	}

	list->size--;

	return idx;

}

int32_t int_array_list_remove(IntArrayList* list, int32_t data){

	int32_t idx = int_array_list_search(list, data);
	return int_array_list_remove_at(list, idx);
}

void int_array_list_extend(IntArrayList* dest, IntArrayList* src){
	int32_t cmbsize = dest->size + src->size;
	
	if(cmbsize >= dest->size){
		array_list_grow(dest);
	}

	int32_t* ddata = (int32_t *)dest->data;
	int32_t* sdata = (int32_t *)src->data;
	if(dest->data != NULL){
		int32_t* to = ddata + dest->size;
		memcpy(to, sdata, (src->size)*sizeof(int32_t));
	}

}

void int_array_list_delete(IntArrayList* list){
	array_list_delete(list);
}

int32_t* int_array_list_data(IntArrayList* list, int32_t idx){
	int32_t* it = (int32_t *) array_list_value_at(list, idx);
	return it;
}

int32_t int_array_list_set_value_at(IntArrayList* list, int32_t data, int32_t index)
{
	if(index >= list->size)
	{
		return 0;
	}

	int32_t* ldata = (int32_t *)list->data;
	ldata[index] = data;

	return 1;
}

void int_array_list_sort(IntArrayList* list){
	int32_t* ldata = (int32_t *)list->data;
	qsort(ldata, list->size, sizeof(int32_t), int_compare);
}

