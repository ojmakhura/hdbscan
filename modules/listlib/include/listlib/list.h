/**
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
 * @file list.h 
 * 
 *   Created on: 3 Oct 2017
 *      Author: ojmakh
 * 
 * This file defines the basic structure of an array based list. The 
 * structure ArrayList is designed to handle any data by being 
 * declared to hold void* datatype. We wanted the pointer array to
 * still be able to hold primitive datatypes without using pointers.
 * 
 * We achieve this by using the *step* variable which denotes the 
 * size in bytes of each elelment in *data*. When inserting into the
 * list, we perform a *memcpy* of the data into the array. If saving
 * pointers, this means you should pass a void ** to the inserting
 * into the array. We use the function array_list_insertion_helper() 
 * in list.c to copy the data.
 * 
 * 
 */
#ifndef LIST_H_
#define LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/*************************************************
 * \struct ArrayList
 * 
 * \brief A generic array list capable of storing any
 * datatype. 
 * 
 * It sould be noted that at the lowest
 * level, the void* pointer is addressed byte by
 * byte. As such it is necessary to use the step
 * to jump over the size of the actual data in the
 * array if one wants to work with the void* pointer
 * directly. 
 *************************************************/ 
struct ArrayList{
	void* data;		    /// The pointer to the data
	int32_t size;		/// Current number of items in the list
	int32_t max_size;	/// Maximum number of items that can be stored
	int32_t step;		/// The number of bytes each element takes
};

/**
 * @brief 
 * \typedef ArrayList
 */
typedef struct ArrayList ArrayList; 

/**
 * @brief Initialises the array list. 
 * 
 * The list data pointer is allocated as bytes. It therefore requires 
 * multiplication by the size of the ndividual data elements that will be stored. 
 * 
 * @param initial_size The initial maximum number of elements the array can hold
 * @param step The memory size of each element in the list
 * @return ArrayList* the list or NULL in case of failed allocation.
 */
ArrayList* array_list_init(size_t initial_size, size_t step);

/**
 * @brief Get the pointer to the element at pos. 
 * 
 * The array in the list is treated as an array of bytes. As such 
 * we actually return the pointer to the byte at list->size * list->step.
 * 
 * @param list The list
 * @param pos Position to to get the value from
 * @return void* 
 */
void* array_list_value_at(ArrayList* list, int32_t pos);

/**
 * @brief Grows the list data array by twice the current maximum size. We
 * also make sure that the new size is a power of 2.
 * 
 * @param list The list to grow
 * @return int32_t Returns 1 if successful and -1 if not successful
 */
int32_t array_list_grow(ArrayList* list);

/**
 * @brief Deletes the list. 
 * 
 * Frees the memory for both the data and the list itself. Note the data 
 * deletions is simply deallocation of the array memory. The actual data 
 * in this case is being managed from somewhere else and the user should 
 * be responsible for cleaning that up too.
 * 
 * @param list The list
 * @return ArrayList* Returns a NULL lists if successful
 */
ArrayList* array_list_delete(ArrayList* list);

/**
 * @brief Insert the data at the specified location. 
 * 
 * This function  makes sure you only insert in locations that contain
 * data because we are only replacing what is already there.
 * 
 * @param list The list
 * @param data Pointer to the data to insert
 * @return
 * 
 */
int32_t array_list_insert_at(ArrayList* list, void* data, int32_t pos);

/**
 * @brief Add the data to the end of the list. 
 * 
 * This function should only
 * directly used if working with pointers. If using either using
 * double, int or long, you should use the functions in inlist.h,
 * doublelist.h or longlist.h which are designed for specifically
 * handling the specified datatypes.
 * 
 * If the data you want to save in the list is originally a pointer,
 * you should create a pointer to the pointer and pass it to this
 * function. In that case, data will actually void** data.
 * 
 * @param list The list
 * @param data The pointer to the data to add
 * @return 0 if unsuccessful and 1 otherwise
 */
int32_t array_list_append(ArrayList* list, void* data);


#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_CONTAINERS_LIST_H_ */
