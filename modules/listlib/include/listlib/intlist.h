/*
 * intlist.h
 *
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
 * @file intlist.h 
 * 
 * Created on: 2 Oct 2017
 *      Author: junior
 * 
 */
#ifndef INTLIST_H_
#define INTLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "list.h"

/**************************************************************************
 *
 *
 **************************************************************************/
/**\typedef IntArrayList */
typedef ArrayList IntArrayList;

/**
 * @brief Initialise the integer array list
 * 
 * @return IntArrayList* 
 */
IntArrayList* int_array_list_init();

/**
 * @brief Initialise the integer array list with the given size
 * 
 * The size in this function is a guideline. The function will
 * allocate memory for the next size that is a power of 2.
 * 
 * @param size 
 * @return IntArrayList* 
 */
IntArrayList* int_array_list_init_size(int32_t size);

/**
 * @brief Initialise the integer array list to exactly given size
 * 
 * @param size 
 * @return IntArrayList* 
 */
IntArrayList* int_array_list_init_exact_size(int32_t size);

/**
 * @brief Initialise the ineger array list to be filled by value
 * 
 * @param size 
 * @param value 
 * @return IntArrayList* 
 */
IntArrayList* int_array_list_init_full(int32_t size, int32_t value);

/**
 * @brief Append data to the end of the list
 * 
 * @param list 
 * @param data 
 */
void int_array_list_append(IntArrayList* list, int32_t data);

/**
 * @brief Remove an item from the end of the list.
 * 
 * @param list 
 * @return int32_t 
 */
int32_t int_array_list_pop(IntArrayList* list);

/**
 * @brief Search the list for the data
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t int_array_list_search(IntArrayList* list, int32_t data);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t int_array_list_search_sorted(IntArrayList* list, int32_t data);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t int_array_list_remove(IntArrayList* list, int32_t data);

/**
 * @brief Completely deallocate the whole list
 * 
 * @param list 
 */
void int_array_list_delete(IntArrayList* list);

/**
 * @brief Find the data at idx
 * 
 * @param list 
 * @param idx 
 * @return int32_t* 
 */
int32_t* int_array_list_data(IntArrayList* list, int32_t idx);

/**
 * @brief Sort the list
 * 
 * @param list 
 */
void int_array_list_sort(IntArrayList* list);

/**
 * @brief extend the size of the list
 * 
 * @param list 
 */
void int_array_list_extend(IntArrayList* list);

/**
 * @brief Set the value at location index
 * 
 * The function makes sure that index is not greater than size of the list
 * 
 * @param list 
 * @param data 
 * @param index 
 * @return int32_t 
 */
int32_t int_array_list_set_value_at(IntArrayList* list, int32_t data, int32_t index);

/**
 * @brief remove the data at idx
 * 
 * @param list 
 * @param idx 
 * @return int32_t 
 */
int32_t int_array_list_remove_at(IntArrayList* list, int32_t idx);

#ifdef __cplusplus
}
#endif
#endif /* INTLIST_H_ */
