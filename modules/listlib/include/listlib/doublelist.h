/*
 * doublelist.h
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

#ifndef INCLUDE_CONTAINERS_DOUBLELIST_H_
#define INCLUDE_CONTAINERS_DOUBLELIST_H_


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
typedef ArrayList DoubleArrayList;

DoubleArrayList* double_array_list_init();
DoubleArrayList* double_array_list_init_size(int32_t size);
DoubleArrayList* double_array_list_init_full(int32_t size, double value);
void double_array_list_append(DoubleArrayList* list, double data);
void double_array_list_insert_at(DoubleArrayList* list, double data, int32_t index);
void double_array_list_pop(DoubleArrayList* list);
int32_t double_array_list_search(DoubleArrayList* list, double data);
int32_t double_array_list_search_sorted(DoubleArrayList* list, double data);
int32_t double_array_list_remove(DoubleArrayList* list, double data);
void double_array_list_delete(DoubleArrayList* list);
double* double_array_list_data(DoubleArrayList* list, int32_t idx);
void double_array_list_sort(DoubleArrayList* list);
void double_array_list_extend(DoubleArrayList* dest, DoubleArrayList* src);
void double_array_list_set_value_at(DoubleArrayList* list, double data, int32_t index);
int32_t double_array_list_remove_at(DoubleArrayList* list, double data, int32_t idx);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_CONTAINERS_DOUBLELIST_H_ */
