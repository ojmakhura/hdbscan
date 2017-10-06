/*
 * intlist.h
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
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
