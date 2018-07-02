/*
 * utils.h
 *
 *  Created on: 16 Sep 2017
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
typedef GHashTable LongIntPointerMap;
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
