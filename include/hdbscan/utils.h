/*
 * utils.h
 *
 *  Created on: 16 Sep 2017
 *      Author: junior
 */

#ifndef HDBSCAN_UTILS_H_
#define HDBSCAN_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef GList ListNode;
typedef GList LongList;
typedef GList ListIntList;
typedef GList DoubleList;
typedef GList ListDoubleList;

typedef GList ClusterList;
typedef GList ConstraintList;

typedef GPtrArray IntPtrSet;
typedef GPtrArray IntPtrList;
typedef GPtrArray LongPtrList;
typedef GPtrArray ListIntPtrList;
typedef GPtrArray DoublePtrList;
typedef GPtrArray ListDoublePtrList;

typedef GPtrArray ClusterPtrList;
typedef GPtrArray ConstraintPtrList;

typedef GHashTable IntIntListMap;			// Hash table with keys as int and values as a list of integers
typedef GHashTable LongIntListMap;
typedef GHashTable IntDoubleMap;
typedef GHashTable IntClusterMap;

#ifndef boolean
typedef gint boolean;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/**
 *
 */
gint gint_compare(gconstpointer ptr_a, gconstpointer ptr_b);

IntSet* set_int_insert(IntSet* set, int32_t d, guint* size);
boolean set_int_ptr_insert(IntPtrSet* set, int32_t d);

IntList* list_int_insert(IntList* list, int32_t d);
void list_int_ptr_insert(IntPtrList* list, int32_t d);


void list_int_clean(IntList* list);

IntList* list_full_link_delete(IntList* list, ListNode* l, GDestroyNotify free_func);
#ifdef __cplusplus
}
#endif
#endif /* HDBSCAN_UTILS_H_ */
