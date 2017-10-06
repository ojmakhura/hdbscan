/*
 * utils.c
 *
 *  Created on: 16 Sep 2017
 *      Author: junior
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
