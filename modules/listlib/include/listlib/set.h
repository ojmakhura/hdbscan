/*
 * set.h
 *
 *  Created on: 30 Jun 2019
 *      Author: ojmakh
 * 
 * Copyright 2019 Onalenna Junior Makhura
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
/** @file set.h */
#ifndef SET_H_
#define SET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"

typedef ArrayList set_t;

/**
 * @brief Initialise the set
 * 
 * @param step 
 * @param compare 
 * @return set* 
 */
set_t* set_init(size_t step, int32_t (*compare)(const void *a, const void *b));

/**
 * @brief Deallocate all dynamically allocated memory for the set
 * 
 * @param _set 
 * @return set* 
 */
set_t* set_delete(set_t* _set);

/**
 * @brief Remove all data from the set
 * 
 * @param _set 
 */
void set_clear(set_t* _set);

/**
 * @brief insert data in to the set
 * 
 * @param _set 
 * @param data 
 * @return int32_t 
 */
int32_t set_insert(set_t* _set, void* data);

/**
 * @brief get the set size
 * 
 * @param _set 
 * @return int32_t 
 */
size_t set_size(set_t* _set);

/**
 * @brief Remove data from set
 * 
 * @param _set 
 * @param data 
 * @return int32_t 
 */
int32_t set_remove(set_t* _set, void* data);

/**
 * @brief Find the data in the set
 * 
 * @param _set 
 * @param data 
 * @return int64_t 
 */
int64_t set_find(set_t* _set, void* data);

/**
 * @brief Find the data at index
 * 
 * @param _set 
 * @param index 
 * @param data 
 * @return int32_t 
 */
int32_t set_value_at(set_t* _set, size_t index, void* data);

/**
 * @brief Remove the data at pos
 * 
 * @param _set 
 * @param pos 
 * @return int32_t 
 */
int32_t set_remove_at(set_t* _set, size_t pos, void* data);

void set_sort(set_t* _set);

int32_t set_empty(set_t* _set);

#ifdef __cplusplus
};
#endif
#endif