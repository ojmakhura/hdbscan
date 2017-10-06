/*
 * list.h
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
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
