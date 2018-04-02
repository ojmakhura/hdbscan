/*
 * longlist.c
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
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
#include "listlib/longlist.h"
#include "listlib/utils.h"


/****************************************************************************************
 * Implementation of array lists
 ****************************************************************************************/


LongArrayList* long_array_list_init(){

	return long_array_list_init_size(256);
}

LongArrayList* long_array_list_init_size(int32_t size){

	LongArrayList* list = (LongArrayList*)array_list_init( highestPowerof2(size*2), sizeof(long));

	return list;
}

LongArrayList* long_array_list_init_full(int32_t size, long value){

	LongArrayList* list = long_array_list_init_size(size);
	long* ldata = (long *)list->data;

	for(int32_t i = 0; i < size; i++){
		ldata[i] = value;
	}
	list->size = size;
	return list;
}

void long_array_list_append(LongArrayList* list, long data){

	if(list->size == list->max_size){
		array_list_grow(list);
	}
	long* ldata = (long *)list->data;
	ldata[list->size] = data;
	list->size++;
}


void long_array_list_pop(LongArrayList* list){
	list->size--;
}

int32_t long_array_list_search(LongArrayList* list, long data){
	int32_t idx = -1;

	long* ldata = (long *)list->data;
	for(int32_t i = 0; i < list->size; i++){
		if(ldata[i] == data){
			idx = i;
			break;
		}
	}

	return idx;
}


int32_t long_array_list_search_sorted(LongArrayList* list, long data){

	int32_t idx = -1;
	long* ldata = (long *)list->data;
	long* dptr = bsearch(&data, ldata, sizeof(long), list->size, long_compare);

	if(dptr != NULL){
		idx = dptr - ldata;
	}

	return idx;
}


int32_t long_array_list_remove_at(LongArrayList* list, long data, int32_t idx){
	if(idx < 0 || idx > list->size){
		return -1;
	}

	long* ldata = (long *)list->data;
	long* to = ldata + idx;
	long* from = ldata + idx +1;
	int32_t cpysize = list->size - idx -1;

	memcpy(to, from, cpysize * sizeof(long));

	list->size--;

	return idx;

}

int32_t long_array_list_remove(LongArrayList* list, long data){

	int32_t idx = long_array_list_search(list, data);
	return long_array_list_remove_at(list, data, idx);
}

void long_array_list_insert_at(LongArrayList* list, long data, int32_t index){
	if(list->size == list->max_size){
		array_list_grow(list);
	}

	long* ldata = (long *)list->data;
	long* to = ldata + index + 1;
	long* from = ldata + index;
	int32_t cpysize = list->size - index;

	memcpy(to, from, cpysize * sizeof(long));
	list->size++;

}

void long_array_list_extend(LongArrayList* dest, LongArrayList* src){
	int32_t cmbsize = dest->size + src->size;

	if(cmbsize >= dest->size){
		array_list_grow(dest);
	}

	long* ddata = (long *)dest->data;
	long* sdata = (long *)src->data;
	if(dest->data != NULL){
		long* to = ddata + dest->size;
		memcpy(to, sdata, (src->size)*sizeof(long));
	}

}

void long_array_list_delete(LongArrayList* list){
	array_list_delete(list);
}

long* long_array_list_data(LongArrayList* list, int32_t idx){
	long* it = (long *) array_list_value_at(list, idx);
	return it;
}


void long_array_list_sort(LongArrayList* list){
	long* ldata = list->data;
	qsort(ldata, list->size, sizeof(long), long_compare);
}

void long_array_list_set_value_at(LongArrayList* list, long data, int32_t index){
	long* ldata = (long *)list->data;
	ldata[index] = data;
}

