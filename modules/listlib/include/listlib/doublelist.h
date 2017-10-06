/*
 * doublelist.h
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
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
