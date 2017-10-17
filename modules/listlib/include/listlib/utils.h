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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

typedef GList ListNode;
typedef GList ClusterList;
typedef GList ConstraintList;

typedef GPtrArray ClusterPtrList;
typedef GPtrArray ConstraintPtrList;

typedef GHashTable IntIntListMap;			// Hash table with keys as int and values as a list of integers
typedef GHashTable LongIntListMap;
typedef GHashTable IntDoubleMap;
typedef GHashTable IntDoubleListMap;
typedef GHashTable IntClusterListMap;

#ifndef boolean
typedef int boolean;
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

int32_t highestPowerof2(int32_t n);
int32_t int_compare(const void * ptr_a, const void * ptr_b);
int32_t double_compare(const void * ptr_a, const void * ptr_b);
int32_t short_compare(const void * ptr_a, const void * ptr_b);
int32_t long_compare(const void * ptr_a, const void * ptr_b);
int32_t float_compare(const void * ptr_a, const void * ptr_b);

#ifdef __cplusplus
}
#endif
#endif /* HDBSCAN_UTILS_H_ */
