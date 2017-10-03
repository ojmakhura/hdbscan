/*
 * intset.h
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
 */

#ifndef INTSET_H_
#define INTSET_H_
#include "intlist.h"

typedef IntArrayList IntArraySet;
typedef IntList IntSet;

IntArraySet* int_array_set_init();
IntArraySet* int_array_set_init_size(int32_t size);
void int_array_set_insert(IntArraySet* list, int32_t data);
int32_t int_array_set_insert_sorted(IntArraySet* list, int32_t data);
int32_t int_array_set_search(IntArraySet* list, int32_t data);
int32_t int_array_set_search_sorted(IntArraySet* list, int32_t data);
int32_t int_array_set_remove(IntArraySet* list, int32_t data);
void int_array_set_delete(IntArraySet* list);
int32_t int_array_set_data(IntArraySet* list, int32_t idx);
void int_array_set_sort(IntArraySet* list);


#endif /* INTSET_H_ */
