/**
 * utils.c
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

/**
 * \file utils.c
 * \author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 *
 * \brief Utility functions implementations for use by other HDBSCAN modules
 *
 * \date 2019-10-13
 *
 * \copyright Copyright (c) 2019
 *
 */

#include "hdbscan/utils.h"
#include <stdlib.h>

size_t highestPowerof2(size_t n)
{
	size_t p = (size_t)log2((double)n);
   	return (size_t)pow(2, (double)p+1);
}

int32_t int_compare(const void *ptr_a, const void *ptr_b) {

	int32_t a, b;
	a = *(int32_t *) ptr_a;
	b = *(int32_t *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

int32_t int_ptr_compare(const void *ptr_a, const void *ptr_b)  {

	int32_t *a, *b;
	a = *(int32_t **) ptr_a;
	b = *(int32_t **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t double_compare(const void *ptr_a, const void *ptr_b){

	double a, b;
	a = *(double *) ptr_a;
	b = *(double *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);

}

int32_t double_ptr_compare(const void *ptr_a, const void *ptr_b){

	double *a, *b;
	a = *(double **) ptr_a;
	b = *(double **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
    
	return (-1);

}

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t short_compare(const void *ptr_a, const void *ptr_b){

	short a, b;
	a = *(short *) ptr_a;
	b = *(short *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);
}

int32_t short_ptr_compare(const void *ptr_a, const void *ptr_b){

	short *a, *b;
	a = *(short **) ptr_a;
	b = *(short **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
    
	return (-1);

}

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t long_compare(const void * ptr_a, const void * ptr_b){
	long a, b;
	a = *(long *) ptr_a;
	b = *(long *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);
}

int32_t long_ptr_compare(const void * ptr_a, const void * ptr_b){
	long *a, *b;
	a = *(long **) ptr_a;
	b = *(long **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
    
	return (-1);
}

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t float_compare(const void *ptr_a, const void *ptr_b){
	float a, b;
	a = *(float *) ptr_a;
	b = *(float *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);
}

int32_t float_ptr_compare(const void *ptr_a, const void *ptr_b){
	float *a, *b;
	a = *(float **) ptr_a;
	b = *(float **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
    
	return (-1);
}

/**
 * @brief 
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t char_compare(const void *ptr_a, const void *ptr_b){
	char a, b;
	a = *(char *) ptr_a;
	b = *(char *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);
}

int32_t char_ptr_compare(const void *ptr_a, const void *ptr_b){
	char *a, *b;
	a = *(char **) ptr_a;
	b = *(char **) ptr_b;

	if (*a > *b) {
		return (1);
	}
	if (*a == *b) {
		return (0);
	}
    
	return (-1);
}

/**
 * @brief For pointers we just compare memory locations
 * 
 * @param ptr_a 
 * @param ptr_b 
 * @return int32_t 
 */
int32_t ptr_compare(const void *ptr_a, const void *ptr_b){
	void *a = *(void **) ptr_a;
	void *b = *(void **) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
    
	return (-1);
}


size_t get_htype_size(enum HTYPES type)
{
	if(type == H_DOUBLE) {
        return sizeof(double);
    } else if(type == H_FLOAT) {
		return sizeof(float);
    } else if(type == H_LONG) {
		return sizeof(long);
    } else if(type == H_SHORT) {
		return sizeof(short);
    }else if(type == H_INT) {
		return sizeof(int32_t);
    } else if(type == H_CHAR) {
		return sizeof(char);
    } 
	
	return sizeof(void *); /// Other wise it is a pointer
}

size_t int_hash(void* key, size_t buckets)
{
	int32_t k = (*(int32_t *)key);
	size_t tmp = (size_t)k;
    return tmp % buckets;
}

size_t long_hash(void* key, size_t buckets)
{
	long k = (*(long *)key);
	size_t tmp = (size_t)k;
    return tmp % buckets;
}

size_t short_hash(void* key, size_t buckets)
{
	short k = (*(short *)key);
	size_t tmp = (size_t)k;
    return tmp % buckets;
}

size_t char_hash(void* key, size_t buckets)
{
    char *db = (char *)key;
	size_t tmp = (size_t)(*db);
    return tmp % buckets;
}

size_t double_hash(void* key, size_t buckets)
{
    double *db = (double *)key;
	size_t tmp = (size_t)(*db);
    return tmp % buckets;
}

size_t float_hash(void* key, size_t buckets)
{
    float *db = (float *)key;
	size_t tmp = (size_t)(*db);
    return tmp % buckets;
}

size_t str_hash(void *key, size_t buckets)
{
    const char *cptr = key;
    unsigned int val = 0;

    while(*cptr != '\0')
    {
        uint tmp;
        val = (val << 4) + (uint)(*cptr);
        tmp = (val & 0xf0000000);
        if(tmp)
        {
            val = val ^ (uint)(tmp >> 24);
            val = val ^ tmp;
        }

         cptr++;
    }
    
	size_t tmp = (size_t)val;

    return tmp % buckets;
}
