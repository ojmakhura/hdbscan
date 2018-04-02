/*
 * disctance.c
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
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <math.h>

#include "hdbscan/distance.h"

double (*get_diff)(distance* dis, void* dataset, uint i, uint j, uint k);

double get_double_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	double* dt = (double*)dataset;
    double num1 = dt[i * dis->cols + k];
    double num2 = dt[j * dis->cols + k];
	return (double)(num1 - num2);
}

double get_int_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	int* dt = (int*)dataset;
    int num1 = dt[i * dis->cols + k];
    int num2 = dt[j * dis->cols + k];
	return (double)(num1 - num2);
}

double get_float_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	float* dt = (float*)dataset;
    float num1 = dt[i * dis->cols + k];
    float num2 = dt[j * dis->cols + k];
    
	return (double)(num1 - num2);
}

double get_long_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	long* dt = (long*)dataset;
    long num1 = dt[i * dis->cols + k];
    long num2 = dt[j * dis->cols + k];
	return (double)(num1 - num2);
}

double get_short_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	short* dt = (short*)dataset;
    short num1 = dt[i * dis->cols + k];
    short num2 = dt[j * dis->cols + k];
	return (double)(num1 - num2);
}

int cmpint(const void * ptr_a, const void * ptr_b) {

	int a, b;
	a = *(int *) ptr_a;
	b = *(int *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

int cmpdouble(const void * ptr_a, const void * ptr_b) {
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

/**
 *
 */
uint triangular(uint n) {
	return (n * n + n) / 2;
}

distance* distance_init(distance* dis, calculator cal, uint datatype) {
	if(dis == NULL)
		dis = (distance*)malloc(sizeof(distance));

	if(dis == NULL){
		printf("ERROR: distance_init - Failed to allocate memory for distance");
	} else{
		dis->cal = cal;
		dis->numNeighbors = 2; /// set the distance to minimum possible neighbors
		dis->coreDistances = NULL;
		dis->distances = NULL;
		dis->datatype = datatype;
		
		if(dis->datatype == DATATYPE_FLOAT){
			get_diff = get_float_diff;
			
		} else if(dis->datatype == DATATYPE_DOUBLE){
			get_diff = get_double_diff;
			
		}else if(dis->datatype == DATATYPE_INT){
			
			get_diff = get_int_diff;

		}	else if(dis->datatype == DATATYPE_LONG){
			
			get_diff = get_long_diff;

		}else if(dis->datatype == DATATYPE_SHORT){
			
			get_diff = get_short_diff;			
		}
	}
	return dis;
}

/**
 *
 */
void distance_destroy(distance* d) {
	distance_clean(d);
	if(d != NULL)
		free(d);
}

void distance_clean(distance* d){
	if(d->distances != NULL){
		free(d->distances);
		d->distances = NULL;
	}

	if(d->coreDistances != NULL){
		free(d->coreDistances);
		d->coreDistances = NULL;
	}
}

/**
 *
 */
double l2_norm(double const* u, uint n) {
	double accum = 0.;

	if (n == 1) {
		return u[0];
	}

	for (uint i = 0; i < n; ++i) {
		accum += u[i] * u[i];
	}
	return sqrt(accum);
}

/**
 *
 */
void do_euclidean(distance* dis, void* dataset) {

	double sortedDistance[dis->rows];
#pragma omp parallel for private(sortedDistance)
	for (uint i = 0; i < dis->rows; i++) {
		for (uint j = 0; j < dis->rows; j++) {
			double sum, diff = 0.0;
			uint offset1;
			sum = 0;

            for (uint k = 0; ((k < dis->cols) && (i != j)); k++) {
    			diff = get_diff(dis, dataset, i, j, k);
    			sum += (diff * diff);
            }

			sum = sqrt(sum);

			int c;
			if (j > i) {
				// Calculate the linearised upper triangular matrix offset
				offset1 = i * dis->rows + j;
				c = offset1 - triangular(i + 1);

				dis->distances[c] = sum;
			} else if (i == j) {
				c = -1;
			} else {
				offset1 = j * dis->rows + i;
				c = offset1 - triangular(j + 1);
			}

			sortedDistance[j] = sum;

		}
		qsort(sortedDistance, dis->rows, sizeof(double), cmpdouble);
		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];
	}

}

double distance_get(distance* dis, uint row, uint col) {
	uint idx;
	if (row < col) {
		idx = (dis->rows * row + col) - triangular(row + 1);

	} else if (row == col) {
		return 0;
	} else {
		idx = (dis->rows * col + row) - triangular(col + 1);
	}
	return dis->distances[idx];
}
void setDimenstions(distance* dis, int rows, int cols){

    dis->rows = rows;
    dis->cols = cols;
    int sub = (rows * rows -rows)/2;
    dis->distances = (double *)malloc(sub * sizeof(double));
    dis->coreDistances = (double *)malloc(dis->rows * sizeof(double));
}

void distance_compute(distance* dis, void* dataset, int rows, int cols, int numNeighbors){
	dis->numNeighbors = numNeighbors;
	setDimenstions(dis, rows, cols);
	
#pragma omp parallel for 
	for (uint i = 0; i < dis->rows; i++) {
		for (uint j = i; j < dis->rows; j++) {
			double sum, diff = 0.0;
			uint offset1;
			sum = 0;

            for (uint k = 0; ((k < dis->cols) && (i != j)); k++) {
    			diff = get_diff(dis, dataset, i, j, k);
    			sum += (diff * diff);
            }

			sum = sqrt(sum);

			int c;
			if (j > i) {
				// Calculate the linearised upper triangular matrix offset
				offset1 = i * dis->rows + j;
				c = offset1 - triangular(i + 1);

				dis->distances[c] = sum;
			} else if (i == j) {
				c = -1;
			} else {
				offset1 = j * dis->rows + i;
				c = offset1 - triangular(j + 1);
			}
		}
	}
	distance_get_core_distances(dis);
}

void distance_get_core_distances(distance *dis){
	
	double sortedDistance[dis->rows];
#pragma omp parallel for private(sortedDistance)
	for (uint i = 0; i < dis->rows; i++) {
		for (uint j = 0; j < dis->rows; j++) {
			sortedDistance[j] = distance_get(dis, i, j);
		}	
		qsort(sortedDistance, dis->rows, sizeof(double), cmpdouble);
		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];	
	}
}
