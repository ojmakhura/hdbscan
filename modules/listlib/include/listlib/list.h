/*
 * list.h
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

#ifndef INCLUDE_CONTAINERS_LIST_H_
#define INCLUDE_CONTAINERS_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/*************************************************
 * A generic array list capable of storing any
 * datatype. It sould be noted that at the lowest
 * level, the void* pointer is addressed byte by
 * byte. As such it is necessary to use the step
 * to jump over the size of the actual data in the
 * array if one wants to work with the void* pointer
 * directly. 
 *************************************************/ 
typedef struct ArrayList{
	void* data;		    /// The pointer to the data
	int32_t size;		/// Current number of items in the list
	int32_t max_size;	/// Maximum number of items that can be stored
	int32_t step;		/// The number of bytes each element takes
} ArrayList;

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
ArrayList* array_list_init(size_t initial_size, size_t step);

/**
 * Get the pointer to the element at pos. The array in the list is treated as an
 * array of bytes. As such we actually return the pointer to the byte at 
 * list->size * list->step.
 * 
 * @param list: the list
 * @param pos
 */
void* array_list_value_at(ArrayList* list, int32_t pos);

/**
 * Grows the list data array by twice the current maximum size. We
 * also make sure that the new size is a power of 2.
 * 
 * @param 
 */
int32_t array_list_grow(ArrayList* list);

/**
 * Deletes the list. Frees the memory for both the data and the list itself. Note the data deletions is simply deallocation
 * of the array memory. The actual data in this case is being managed
 * from somewhere else and the user should be responsible for cleaning 
 * that up too.
 * 
 * @param list
 * @return
 */
ArrayList* array_list_delete(ArrayList* list);

/**
 * Insert the data at the specified location. This function
 * makes sure you only insert in locations that contain data
 * because we are only replacing what is already there.
 * 
 * @param list
 * @param data - pointer to the data
 * 
 */
int32_t array_list_insert_at(ArrayList* list, void* data, int32_t pos);

/**#
 * Add the data to the end of the list. This function should only
 * directly used if working with pointers. If using either using
 * double, int or long, you should use the functions in inlist.h,
 * doublelist.h or longlist.h which are designed for specifically
 * handling the specified datatypes.
 * 
 * If the data you want to save in the list is originally a pointer,
 * you should create a pointer to the pointer and pass it to this
 * function. In that case, data will actually void** data.
 * 
 * @param list
 * @param data
 * @return 0 if unsuccessful and 1 otherwise
 */
int32_t array_list_append(ArrayList* list, void* data);

/**
 * Inserting data at a specified locatin in the ararylist's data array.
 * Normally
 */ 
//int32_t array_list_insertion_helper(ArrayList* list, void* data, int32_t index);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_CONTAINERS_LIST_H_ */
