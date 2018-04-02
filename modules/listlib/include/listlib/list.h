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

typedef struct ArrayList{
	void* data;
	int32_t size;
	int32_t max_size;
	size_t step;
} ArrayList;

/**
 * Initialises the array list. The initial size of the list is always calculated to be
 * the largest power of 2 that is less than initial_size * 2
 *
 * @param initial_size: the initial maximum number of elements the array can hold
 * @param step: the memory size of each element in the list
 */
ArrayList* array_list_init(size_t initial_size, size_t step);

/**
 * Get the pointer to the element at pos. The array in the list is treated as an
 * array of bytes. As such we actually return the pointer to the byte at list->size * list->step
 * @param list: the list
 * @param pos
 */
void* array_list_value_at(ArrayList* list, int32_t pos);

/**
 * Grows the list by twice the current maximum size
 */
int32_t array_list_grow(ArrayList* list);

/**
 * Deletes the list. Frees the memory for both the data and the list itself;
 */
ArrayList* array_list_delete(ArrayList* list);

/**
 *
 */
int32_t array_list_add_at(ArrayList* list, void* data, int32_t pos);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_CONTAINERS_LIST_H_ */
