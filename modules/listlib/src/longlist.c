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

/**
 * @file longlist.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of a long array list
 * 
 * @version 3.1.6
 * @date 2017-10-03
 * 
 * @copyright Copyright (c) 2017
 * 
 */
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "listlib/longlist.h"
#include "hdbscan/utils.h"

#include "config.h"
#ifdef USE_OMP
#include <omp.h>
#endif

/****************************************************************************************
 * Implementation of array lists
 ****************************************************************************************/

LongArrayList* long_array_list_init(){

	return long_array_list_init_size(128);
}

LongArrayList* long_array_list_init_size(int32_t size){

	LongArrayList* list = (LongArrayList*)array_list_init( highestPowerof2(size*2), sizeof(long), long_compare);

	return list;
}

LongArrayList* _array_list_init_exact_size(int32_t size)
{
	return (LongArrayList*)array_list_init(size, sizeof(long), long_compare);
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

void long_array_list_extend(LongArrayList* list){
	int32_t cmbsize = list->size + list->size;

	if(cmbsize >= list->size){
		array_list_grow(list);
	}
}

void long_array_list_delete(LongArrayList* list){
	array_list_delete(list);
}

int32_t long_array_list_data(LongArrayList* list, int32_t idx, long* it){
	return array_list_value_at(list, idx, it);
}


void long_array_list_sort(LongArrayList* list){
	long* ldata = list->data;
	qsort(ldata, list->size, sizeof(long), long_compare);
}

int32_t long_array_list_set_value_at(LongArrayList* list, long data, int32_t index){
	
	if(index >= list->size)
	{
		return 0;
	}
	
	long* ldata = (long *)list->data;
	ldata[index] = data;

	return 1;
}

