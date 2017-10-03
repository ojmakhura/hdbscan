/*
 * intlist.h
 *
 *  Created on: 2 Oct 2017
 *      Author: junior
 */

#ifndef INTLIST_H_
#define INTLIST_H_

#include <stdint.h>
#include <stdlib.h>

int32_t cmpint(const void * ptr_a, const void * ptr_b);
typedef struct Node{
	int32_t data;
	struct Node* next;
	struct Node* prev;
} node;

typedef struct IntList{
	int32_t size;
	node* head;
	node* tail;
} IntList;

IntList* int_list_init();

node* int_list_add(IntList* list, int32_t data);
node* int_list_remove(IntList* list, int32_t data);
void int_list_remove_full(IntList* list, int32_t data);
node* int_list_search(IntList* list, int32_t data);
void int_list_delete(IntList* list);

/**************************************************************************
 *
 *
 **************************************************************************/
typedef struct IntArrayList{
	int32_t* data;
	int32_t size;
	int32_t max_size;
} IntArrayList;

int32_t list_extend_array(IntArrayList* list);;
IntArrayList* int_array_list_init();
IntArrayList* int_array_list_init_size(int32_t size);
void int_array_list_append(IntArrayList* list, int32_t data);
void int_array_list_insert_at(IntArrayList* list, int32_t data, int32_t index);
void int_array_list_pop(IntArrayList* list);
int32_t int_array_list_search(IntArrayList* list, int32_t data);
int32_t int_array_list_search_sorted(IntArrayList* list, int32_t data);
int32_t int_array_list_remove(IntArrayList* list, int32_t data);
void int_array_list_delete(IntArrayList* list);
int32_t int_array_list_data(IntArrayList* list, int32_t idx);
void int_array_list_sort(IntArrayList* list);
void int_array_list_extend(IntArrayList* dest, IntArrayList* src);


/**************************************************************************
 *
 *
 **************************************************************************/
/*typedef struct IntPtrList{
	node* data;
	int32_t size;
	int32_t max_size;
} IntPtrList;

IntPtrList* int_ptr_list_init();
IntPtrList* int_ptr_list_init_size(int32_t size);
void int_ptr_list_add(IntPtrList* list, int32_t data);
int32_t int_ptr_list_search(IntPtrList* list, int32_t data);
int32_t int_ptr_list_remove(IntPtrList* list, int32_t data);
void int_ptr_list_delete(IntPtrList* list);*/

#endif /* INTLIST_H_ */
