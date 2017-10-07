/*
 * disctance.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>
#include <math.h>

#include "hdbscan/distance.h"

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
static inline
double get_diff(distance* dis, void* dataset, uint i, uint j, uint k){
	double diff = 0;
	double num1 = 0, num2 = 0;

	if(dis->datatype == DATATYPE_FLOAT){
		float* dt = (float*)dataset;
		num1 = (double)(dt[i * dis->cols + k]);
		num2 = (double)(dt[j * dis->cols + k]);
	} else if(dis->datatype == DATATYPE_DOUBLE){
		double* dt = (double*)dataset;
    	num1 = dt[i * dis->cols + k];
    	num2 = dt[j * dis->cols + k];
	}else if(dis->datatype == DATATYPE_INT){
		int* dt = (int*)dataset;
    	num1 = (double)(dt[i * dis->cols + k]);
    	num2 = (double)(dt[j * dis->cols + k]);

	}	else if(dis->datatype == DATATYPE_LONG){
		long* dt = (long*)dataset;
    	num1 = (double)(dt[i * dis->cols + k]);
    	num2 = (double)(dt[j * dis->cols + k]);

	}else if(dis->datatype == DATATYPE_SHORT){
		short* dt = (short*)dataset;
    	num1 = (double)(dt[i * dis->cols + k]);
    	num2 = (double)(dt[j * dis->cols + k]);
	}
	diff = num1 - num2;
	return diff;
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
		//#pragma omp barrier
		qsort(sortedDistance, dis->rows, sizeof(double), cmpdouble);
		//printf("dis->coreDistances[%d] = %f and mnumNeighbors = %d\n", i, sortedDistance[dis->numNeighbors], dis->numNeighbors);
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
	//do_euclidean(dis, dataset);
	
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
		//#pragma omp barrier
		qsort(sortedDistance, dis->rows, sizeof(double), cmpdouble);
		//printf("dis->coreDistances[%d] = %f and mnumNeighbors = %d\n", i, sortedDistance[dis->numNeighbors], dis->numNeighbors);
		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];
	}
}

