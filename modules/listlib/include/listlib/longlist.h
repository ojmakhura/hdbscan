
/*
 * longlist.h
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

/** @file longlist.h */
#ifndef CONTAINERS_LONGLIST_H_
#define CONTAINERS_LONGLIST_H_


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
/**\typedef LongArrayList */
typedef ArrayList LongArrayList;

/**
 * @brief 
 * 
 * @return LongArrayList* 
 */
LongArrayList* long_array_list_init();

/**
 * @brief 
 * 
 * @param size 
 * @return LongArrayList* 
 */
LongArrayList* long_array_list_init_size(int32_t size);

/**
 * @brief 
 * 
 * @param size 
 * @return LongArrayList* 
 */
LongArrayList* long_array_list_init_exact_size(int32_t size);

/**
 * @brief 
 * 
 * @param size 
 * @param value 
 * @return LongArrayList* 
 */
LongArrayList* long_array_list_init_full(int32_t size, long value);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 */
void long_array_list_append(LongArrayList* list, long data);

/**
 * @brief 
 * 
 * @param list 
 */
void long_array_list_pop(LongArrayList* list);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t long_array_list_search(LongArrayList* list, long data);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t long_array_list_search_sorted(LongArrayList* list, long data);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @return int32_t 
 */
int32_t long_array_list_remove(LongArrayList* list, long data);

/**
 * @brief 
 * 
 * @param list 
 */
void long_array_list_delete(LongArrayList* list);

/**
 * @brief 
 * 
 * @param list 
 * @param idx 
 * @return long* 
 */
int32_t long_array_list_data(LongArrayList* list, int32_t idx, long* it);

/**
 * @brief 
 * 
 * @param list 
 */
void long_array_list_sort(LongArrayList* list);

/**
 * @brief 
 * 
 * @param list 
 */
void long_array_list_extend(LongArrayList* list);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @param index 
 * @return int32_t 
 */
int32_t long_array_list_set_value_at(LongArrayList* list, long data, int32_t index);

/**
 * @brief 
 * 
 * @param list 
 * @param data 
 * @param idx 
 * @return int32_t 
 */
int32_t long_array_list_remove_at(LongArrayList* list, long data, int32_t idx);

#ifdef __cplusplus
}
#endif



#endif /* CONTAINERS_LONGLIST_H_ */
