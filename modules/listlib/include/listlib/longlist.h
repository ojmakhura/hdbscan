
/*
 * longlist.h
 *
 *  Created on: 3 Oct 2017
 *      Author: ojmakh
 */

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
typedef ArrayList LongArrayList;

LongArrayList* long_array_list_init();
LongArrayList* long_array_list_init_size(int32_t size);
LongArrayList* long_array_list_init_full(int32_t size, long value);
void long_array_list_append(LongArrayList* list, long data);
void long_array_list_insert_at(LongArrayList* list, long data, int32_t index);
void long_array_list_pop(LongArrayList* list);
int32_t long_array_list_search(LongArrayList* list, long data);
int32_t long_array_list_search_sorted(LongArrayList* list, long data);
int32_t long_array_list_remove(LongArrayList* list, long data);
void long_array_list_delete(LongArrayList* list);
long* long_array_list_data(LongArrayList* list, int32_t idx);
void long_array_list_sort(LongArrayList* list);
void long_array_list_extend(LongArrayList* dest, LongArrayList* src);
void long_array_list_set_value_at(LongArrayList* list, long data, int32_t index);
int32_t long_array_list_remove_at(LongArrayList* list, long data, int32_t idx);

#ifdef __cplusplus
}
#endif



#endif /* CONTAINERS_LONGLIST_H_ */
