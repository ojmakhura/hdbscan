/*
 * intlist.h
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
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
typedef ArrayList IntArrayList;

IntArrayList* int_array_list_init();
IntArrayList* int_array_list_init_size(int32_t size);
IntArrayList* int_array_list_init_full(int32_t size, int32_t value);
void int_array_list_append(IntArrayList* list, int32_t data);
void int_array_list_insert_at(IntArrayList* list, int32_t data, int32_t index);
int32_t int_array_list_pop(IntArrayList* list);
int32_t int_array_list_search(IntArrayList* list, int32_t data);
int32_t int_array_list_search_sorted(IntArrayList* list, int32_t data);
int32_t int_array_list_remove(IntArrayList* list, int32_t data);
void int_array_list_delete(IntArrayList* list);
int32_t* int_array_list_data(IntArrayList* list, int32_t idx);
void int_array_list_sort(IntArrayList* list);
void int_array_list_extend(IntArrayList* dest, IntArrayList* src);
void int_array_list_set_value_at(IntArrayList* list, int32_t data, int32_t index);
int32_t int_array_list_remove_at(IntArrayList* list, int32_t idx);

#ifdef __cplusplus
}
#endif
#endif /* INTLIST_H_ */
