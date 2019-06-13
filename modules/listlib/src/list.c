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

#include "listlib/list.h"
#include <string.h>
#include <assert.h>
#include "listlib/utils.h"

/**
 * Initialises the array list. The list data pointer is allocated as
 * bytes. It therefore requires multiplication by the size of the
 * individual data elements that will be stored. 
 *
 * @param initial_size: the initial maximum number of elements the array can hold
 * @param step: the memory size of each element in the list
 * 
 * @return the list or NULL in case of failed allocation.
 */
ArrayList* array_list_init(size_t initial_size, size_t step)
{
	assert(initial_size > 0);
	ArrayList* list = (ArrayList *)malloc(sizeof(ArrayList));

	if(list != NULL){
		list->data = malloc(initial_size * step);

		if(list->data == NULL){
			free(list);	// Since the data memory was no allocated, we should
						// clean up the memory allocated for the list.
			return NULL;
		}

		list->max_size = initial_size;
		list->size = 0;
		list->step = step;
	}

	return list;
}

/**
 * Get the pointer to the element at pos. The array in the list is treated as an
 * array of bytes. As such we actually return the pointer to the byte at 
 * list->size * list->step.
 * 
 * @param list: the list
 * @param pos
 */
void* array_list_value_at(ArrayList* list, int32_t pos)
{
	if(pos < 0 || pos >= list->size){
		return NULL;
	}

	size_t p = pos * list->step; /// Calculate the location of the proper byte
	return list->data + p;
}

/**
 * Grows the list data array by twice the current maximum size. We
 * also make sure that the new size is a power of 2.
 * 
 * @param 
 */
int32_t array_list_grow(ArrayList* list){

	int32_t newmax = highestPowerof2(list->max_size*2);
	void* newdata = realloc(list->data, newmax * list->step);

	if(newdata == NULL){
		return -1;
	}

	list->data = newdata;
	list->max_size = newmax;

	return 1;
}

/**
 * Delete the list. Note the data deletions is simply deallocation
 * of the array memory. The actual data in this case is being managed
 * from somewhere else and the user should be responsible for cleaning 
 * that up too.
 */ 
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


/**
 * Helper function for inserting at the location index in the list's
 * array.
 */ 
int32_t array_list_insertion_helper(ArrayList* list, void* data, int32_t index)
{
	// Calculate the location of the byte where to start the
	// insertion
	size_t p = index * list->step;

	// Memory location of the byte where to start copying.
	void* np = list->data + p;

	// Copy the 'step' bytes from data into np
	memcpy(np, data, list->step);
	return 1;
}

/**
 * Insert the data at the specified location. This function
 * makes sure you only insert in locations that contain data
 * because we are only replacing what is already there. 
 */ 
int32_t array_list_insert_at(ArrayList* list, void* data, int32_t index){

	if(index < 0 || index >= list->size)
	{
		return 0;
	}

	return array_list_insertion_helper(list, data, index);
}

/**
 * Append the data to the end of the list. If the data
 * you want to save in the list is originally a pointer,
 * you should create a pointer to the pointer and pass
 * it to this function. In that case, data will actually
 * void** data.
 */ 
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