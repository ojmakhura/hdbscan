/*
 * list.c
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
 * @file list.c
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief This file contains the implementation of the functions
 * declared in list.h header.
 * 
 * @version 3.1.6
 * @date 2017-10-03
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include "listlib/list.h"
#include <string.h>
#include <assert.h>
#include "hdbscan/utils.h"

#include "config.h"
#ifdef USE_OMP
#include <omp.h>
#endif

ArrayList* array_list_init(size_t initial_size, size_t step, int32_t (*compare)(const void *a, const void *b))
{
	assert(initial_size > 0);
	ArrayList* list = (ArrayList *)malloc(sizeof(ArrayList));

	

	if(list != NULL){
		size_t s = initial_size * step;
		list->data = malloc(s); /// Make sure all values are NULL/0

		#ifdef USE_OMP
		#pragma omp parallel for
		#endif
		for(size_t i = 0; i < s; i++)
		{
			((char *)list->data)[i] = (char)0;
		}

		if(list->data == NULL){
			free(list);	// Since the data memory was no allocated, we should
						// clean up the memory allocated for the list.
			return NULL;
		}
	}

	list->max_size = initial_size;
	list->size = 0;
	list->step = step;
	list->compare = compare;

	return list;
}

ArrayList* ptr_array_list_init(size_t initial_size, int32_t (*compare)(const void *a, const void *b))
{
	return array_list_init(initial_size, sizeof(void *), compare);
}

/**
 * @brief Returns the pointer at pos
 * 
 * Care should be taken with this since we give back exactly what you gave us.
 * 
 * @param list 
 * @param pos
 * @param data
 * @return int32_t
 */
int32_t array_list_value_at(ArrayList* list, size_t pos, void* data)
{
	if(pos < 0 || pos >= list->size){
		return 0;
	}

	size_t p = pos * list->step; /// Calculate the location of the proper byte
	void* d = ((char *)list->data) + p;

	memcpy(data, d, list->step);

	return 1; 
}

/**
 * @brief 
 * 
 * @param list 
 * @return int32_t 
 */
int32_t array_list_grow(ArrayList* list){

	int32_t newmax = highestPowerof2(list->max_size*2);
	list->data = realloc(list->data, newmax * list->step);

	if(list->data == NULL){
		return -1;
	}

	list->max_size = newmax;

	return 1;
}

/**
 * @brief 
 * 
 * @param list 
 * @return ArrayList* 
 */
ArrayList* array_list_delete(ArrayList* list){
	if(list != NULL){ /// Make sure the list is not NULL
		if(list->data != NULL){
			free(list->data);		/// Free the data pointer
		}

		free(list);				/// Free the ArrayList structure
		list = NULL;
	}

	return NULL;
}

/**
 * @brief Helper function for inserting at the location index in the list's
 * array. 
 * 
 * We use *memcpy* to copy list->step bytes from *data* into *list->data*
 * at location index * list->step. To make sure of datatype consistency,
 * we first cast list->data to a char * in order to count byte by byte.
 * 
 * @param list The list
 * @param data The data
 * @param index The location where to insert
 * @return int32_t 
 */
int32_t array_list_insertion_helper(ArrayList* list, void* data, size_t index)
{
	// Calculate the location of the byte where to start the
	// insertion
	size_t p = index * list->step;

	// Memory location of the byte where to start copying.
	void* np = ((char *)list->data) + p;

	// Copy the 'step' bytes from data into np
	memcpy(np, data, list->step);
	
	return 1;
}

int32_t array_list_insert_at(ArrayList* list, void* data, size_t index){

	if(index == 0 || index >= list->size)
	{
		return 0;
	}

	return array_list_insertion_helper(list, data, index);
}

int32_t array_list_append(ArrayList* list, void* data)
{
	if(list->size == list->max_size){
		int32_t r = array_list_grow(list);
		if(r < 0){
			return 0;
		}
	}

	// When appending, we are inserting at the list->size index
	if(!array_list_insertion_helper(list, data, list->size))
	{
		return 0;
	}
	list->size++;

	return 1;
}

size_t array_list_size(ArrayList* list)
{
	return list == NULL ? 0 : list->size;
}

int32_t array_list_find(ArrayList* list, void* data)
{
	int32_t tmp = -1;
	void* t = malloc(list->step);
	for(size_t i = 0; i < list->size; i++)
	{
		if(!array_list_value_at(list, i, t))
		{
			return -1;
		}

		if(list->compare(t, data) == 0)
		{
			tmp = i;
			break;
		}
	}

	free(t);

	return tmp;
}

void array_list_clear(ArrayList* list, int32_t resize)
{
	list->size = 0;
	if(resize) {
		list->data = realloc(list->data, list->step);
	}
}

int32_t array_list_remove_at(ArrayList* list, size_t pos, void* data)
{
	if(pos < 0 || pos >= list->size)
	{
		data = NULL;
		return 0;
	}


	// Create byte array versions list->data and data
	char* dt = (char *)list->data;
	char* d1 = (char *)data;
	// Copy the some data from list->data to data
	for(size_t i = 0; i < list->step; i++)
	{
		d1[i] = dt[pos + 1];
	}
	
	size_t last = list->size * list->step;
	for(size_t pos_b = pos * list->step; pos_b < last; pos_b++)
	{
		dt[pos_b] = dt[pos_b + list->step];
	}

	list->size--;

	return 1;
}

int32_t array_list_pop(ArrayList* list, void* data)
{
	if(array_list_empty(list))
	{
		return 0;
	}

	list->size--;
	
	return 1;
}

int32_t array_list_empty(ArrayList* list)
{
	if(list == NULL || list->data == NULL || list->size == 0)
	{
		return 1;
	}

	return 0;
}

/**
 * @brief 
 * 
 * @param list 
 */
void array_list_sort(ArrayList* list){

	qsort(list->data, list->size, list->step, list->compare);
}