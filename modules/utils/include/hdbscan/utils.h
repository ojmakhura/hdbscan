/**
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

/** @file utils.h */
#ifndef HDBSCAN_UTILS_H_
#define HDBSCAN_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

/// \enum Types of data that HDBSCAN handles
enum HTYPES
{
    H_INT,
    H_DOUBLE,
    H_FLOAT,
    H_LONG,
    H_SHORT,
    H_CHAR,
    H_STRING,
    H_PTR
};

/**
 * @brief Get the htype memory size
 * 
 * @param type 
 * @return size_t 
 */
size_t get_htype_size(enum HTYPES type);

/**
 * @brief Find the higherst power of 2 greater than n
 * 
 * @param n 
 * @return int32_t 
 */
int32_t highestPowerof2(int32_t n);

/**
 * @brief Compare integers
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t int_compare(const void * ptr_a, const void * ptr_b) ;
int32_t int_ptr_compare(const void * ptr_a, const void * ptr_b) ;

/**
 * @brief Compare doubles
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t double_compare(const void * ptr_a, const void * ptr_b);
int32_t double_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief Compare short
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t short_compare(const void * ptr_a, const void * ptr_b);
int32_t short_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief Compare long
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t long_compare(const void * ptr_a, const void * ptr_b);
int32_t long_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief Compare float
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t float_compare(const void * ptr_a, const void * ptr_b);
int32_t float_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t char_compare(const void * ptr_a, const void * ptr_b);
int32_t char_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief Compare pointers
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t ptr_compare(const void * ptr_a, const void * ptr_b);
int32_t ptr_ptr_compare(const void * ptr_a, const void * ptr_b);

/**
 * @brief Calculate the triangular number of n
 * 
 * @param n 
 * @return uint 
 */
inline uint TRIANGULAR_H(uint n) {
	return (n * n + n) / 2;
}

/**
 * @brief Find the first triangle number greater than p
 * 
 * @param p 
 * @return uint 
 */
inline uint hostFistTriangleNum(uint p)
{
    return ceil((sqrt(8 * p + 1) - 1) / 2);
}

/**
 * @brief Create a hash of the key.
 * 
 * @param key 
 * @param size 
 * @return int32_t 
 */
int32_t int_hash(void* key, size_t buckets);

int32_t long_hash(void* key, size_t buckets);

int32_t short_hash(void* key, size_t buckets);

int32_t char_hash(void* key, size_t buckets);

int32_t double_hash(void* key, size_t buckets);

int32_t float_hash(void* key, size_t buckets);

int32_t str_hash(void *key, size_t buckets);

/**
 * @brief Compare hashtable entries using keys
 * 
 * @param a 
 * @param b 
 * @return int32_t 
 */
int32_t hashtable_entry_compare(const void* a, const void* b);

void distance_timsort();

#ifdef __cplusplus
}
#endif
#endif /* HDBSCAN_UTILS_H_ */
