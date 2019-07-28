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

ArrayList* array_list_init(size_t initial_size, size_t step)
{
	assert(initial_size > 0);
	ArrayList* list = (ArrayList *)malloc(sizeof(ArrayList));

	if(list != NULL){
		list->data = calloc(initial_size, step); /// Make sure all values are NULL/0

		if(list->data == NULL){
			free(list);	// Since the data memory was no allocated, we should
						// clean up the memory allocated for the list.
			return NULL;
		}
	}

	list->max_size = initial_size;
	list->size = 0;
	list->step = step;

	return list;
}

ArrayList* ptr_array_list_init(size_t initial_size)
{
	return array_list_init(initial_size, sizeof(void *));
}

/**
 * @brief Returns the pointer at pos
 * 
 * Care should be taken with this since we give back exactly what you gave us.
 * 
 * @param list 
 * @param pos 
 * @return void* 
 */
void* array_list_value_at(ArrayList* list, int32_t pos)
{
	if(pos < 0 || pos >= list->size){
		return NULL;
	}

	size_t p = pos * list->step; /// Calculate the location of the proper byte

	return ((char *)list->data) + p; 
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
		list->max_size = 0;
		list->size = 0;
		list = NULL;
	}

	return list;
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
int32_t array_list_insertion_helper(ArrayList* list, void* data, int32_t index)
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

int32_t array_list_insert_at(ArrayList* list, void* data, int32_t index){

	if(index < 0 || index >= list->size)
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

int32_t array_list_size(ArrayList* list)
{
	return list == NULL ? 0 : list->size;
}