/*
 * utils.c
 *
 *  Created on: 16 Sep 2017
 *      Author: junior
 */
#include "hdbscan/utils.h"
#include <glib.h>

gint gint_compare(gconstpointer ptr_a, gconstpointer ptr_b) {
	gint a, b;
	//printf("con")
	a = *(gint *) ptr_a;
	b = *(gint *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

IntSet* set_int_insert(IntSet* set, int d, guint* size){
	ListNode *search = g_list_find_custom(set, &d, (GCompareFunc)gint_compare);
	if(search == NULL){
		int *data = (int *)malloc(sizeof(int));
		*data = d;
		set = g_list_insert_sorted(set, data, gint_compare);
		(*size)++;
	}
	return set;
}

boolean set_int_ptr_insert(IntPtrSet* set, int d){

	gboolean found = FALSE;

	for(guint i = 0; i < set->len; i++){
		int * p = set->pdata[i];
		if(d == *p){
			found = TRUE;
			break;
		}
	}

	if(!found){
		int *data;
		data = (int *)malloc(sizeof(int));
		*data = d;
		g_ptr_array_add(set, data);
		g_ptr_array_sort(set, (GCompareFunc)gint_compare);
	}

	return !found;
}

IntList* list_int_insert(IntList* list, int d){

	int *data = (int *)malloc(sizeof(int));
	*data = d;
	list = g_list_append(list, data);

	return list;
}

void list_int_ptr_insert(IntPtrList* list, int d){

	int *data = (int *)malloc(sizeof(int));
	*data = d;
	g_ptr_array_add(list, data);

}


IntList* list_full_link_delete(IntList* list, ListNode* l, GDestroyNotify free_func){
	list = g_list_remove_link(list, l);
	int* data = (int *)l->data;
	g_list_free_1(l);
	free_func(data);

	return list;
}
