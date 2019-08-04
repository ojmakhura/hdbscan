/*
 * doublelist.c
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
 * @file doublelist.c
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief The double array list implementation
 * 
 * @version 3.1.6
 * @date 2017-10-03
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "listlib/doublelist.h"
#include "hdbscan/utils.h"


/****************************************************************************************
 * Implementation of array lists
 ****************************************************************************************/

DoubleArrayList* double_array_list_init(){

	return double_array_list_init_size(128);
}

DoubleArrayList* double_array_list_init_size(int32_t size){
	assert(size > 0);
	DoubleArrayList* list = (DoubleArrayList*)array_list_init( highestPowerof2(size), sizeof(double), double_compare);

	return list;
}

DoubleArrayList* double_array_list_init_exact_size(int32_t size){
	assert(size > 0);
	DoubleArrayList* list = (DoubleArrayList*)array_list_init(size, sizeof(double), double_compare);

	return list;
}

DoubleArrayList* double_array_list_init_full(int32_t size, double value){
	assert(size > 0);
	DoubleArrayList* list = double_array_list_init_size(size);
	double* ldata = (double *)list->data;
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(int32_t i = 0; i < size; i++){
		ldata[i] = value;
	}
	list->size = size;
	return list;
}

void double_array_list_append(DoubleArrayList* list, double data){
	assert(list != NULL);
	if(list->size == list->max_size){
		array_list_grow(list);
	}
	double* ldata = (double *)list->data;
	ldata[list->size] = data;
	list->size++;
}

void double_array_list_pop(DoubleArrayList* list){
	assert(list != NULL);
	list->size--;
}

int32_t double_array_list_search(DoubleArrayList* list, double data){
	assert(list != NULL);
	int32_t idx = -1;

	double* ldata = (double *)list->data;
	for(int32_t i = 0; i < list->size; i++){
		if(ldata[i] == data){
			idx = i;
			break;
		}
	}

	return idx;
}

int32_t double_array_list_search_sorted(DoubleArrayList* list, double data){
	assert(list != NULL);
	int32_t idx = -1;
	double* ldata = (double *)list->data;
	double* dptr = bsearch(&data, ldata, sizeof(double), list->size, double_compare);

	if(dptr != NULL){
		idx = dptr - ldata;
	}

	return idx;
}

int32_t double_array_list_remove_at(DoubleArrayList* list, double data, int32_t idx){

	assert(list != NULL);
	assert(idx >= 0);
	if(idx < 0 || idx > list->size){
		return -1;
	}
	double* ldata = (double *)list->data;
	double* to = ldata + idx;
	double* from = ldata + idx +1;
	int32_t cpysize = list->size - idx -1;

	memcpy(to, from, cpysize * sizeof(double));

	list->size--;
	return idx;
}

int32_t double_array_list_remove(DoubleArrayList* list, double data){

	assert(list != NULL);
	int32_t idx = double_array_list_search(list, data);
	return double_array_list_remove_at(list, data, idx);
}

void double_array_list_extend(DoubleArrayList* list)
{
	assert(list != NULL);
	int32_t cmbsize = list->size + list->size;

	if(cmbsize >= list->size){
		array_list_grow(list);
	}
}

void double_array_list_delete(DoubleArrayList* list)
{
	assert(list != NULL);
	array_list_delete(list);
}

int32_t double_array_list_data(DoubleArrayList* list, int32_t idx, double* it)
{
	assert(list != NULL && idx >= 0);
	return array_list_value_at(list, idx, it);
}


void double_array_list_sort(DoubleArrayList* list)
{
	assert(list != NULL);
	double* ldata = list->data;
	qsort(ldata, list->size, sizeof(double), double_compare);
}

int32_t double_array_list_set_value_at(DoubleArrayList* list, double data, int32_t index)
{
	assert(list != NULL);
	assert(index >= 0);
	if(index >= list->size)
	{
		return 0;
	}

	double* ldata = (double *)list->data;
	ldata[index] = data;

	return 1;
}

