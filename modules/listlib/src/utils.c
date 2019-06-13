/*
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
#include "listlib/utils.h"
#include <math.h>

int32_t highestPowerof2(int32_t n)
{
	int32_t p = (int32_t)log2(n);
   	return (int32_t)pow(2, p);
}

int32_t int_compare(const void * ptr_a, const void * ptr_b) {

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

int32_t double_compare(const void * ptr_a, const void * ptr_b){

	double a, b;
	a = *(double *) ptr_a;
	b = *(double *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);

}

int32_t short_compare(const void * ptr_a, const void * ptr_b){

	short a, b;
	a = *(short *) ptr_a;
	b = *(short *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);

}

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
	/* default: a < b */
	return (-1);
}

int32_t float_compare(const void * ptr_a, const void * ptr_b){
	float a, b;
	a = *(float *) ptr_a;
	b = *(float *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}
