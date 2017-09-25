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

distance* distance_init(distance* dis, calculator cal) {
	if(dis == NULL)
		dis = (distance*)malloc(sizeof(distance));

	if(dis == NULL){
		printf("ERROR: distance_init - Failed to allocate memory for distance");
	} else{
		dis->cal = cal;
		dis->numNeighbors = 2; /// set the distance to minimum possible neighbors
		dis->coreDistances = NULL;
		dis->distances = NULL;
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
	if(d->distances != NULL)
		free(d->distances);

	if(d->coreDistances != NULL)
		free(d->coreDistances);
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
void do_euclidean(distance* dis, double* dataset) {

	double sortedDistance[dis->rows];
	for (uint i = 0; i < dis->rows; i++) {
		for (uint j = 0; j < dis->rows; j++) {
			double sum, diff = 0.0;
			uint offset1;
			sum = 0;

            for (uint k = 0; ((k < dis->cols) && (i != j)); k++) {
            	double num1 = dataset[i * dis->cols + k];
            	double num2 = dataset[j * dis->cols + k];
    			diff = num1 - num2;
    			sum += (diff * diff);
            }

			sum = sqrt(sum);

			//printf("sum = %f\n", sum);
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
		//qsort()
		qsort(sortedDistance, dis->rows, sizeof(double), cmpdouble);

		/*printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>sorted : %d >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", i);
		for(int j = 0; j < dis->rows; j++){

			printf("%f ", sortedDistance[j]);
		}
		printf("Selecting with %d neighbors for distance %f", dis->numNeighbors, sortedDistance[dis->numNeighbors]);
		printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");*/

		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];
	}

}

double distance_get(distance* dis, uint row, uint col) {
	uint idx;
	if (row < col) {
		//printf("row > col\n");
		idx = (dis->rows * row + col) - triangular(row + 1);

	} else if (row == col) {
		//printf("row == col\n");
		return 0;
	} else {
		//printf("row < col\n");
		idx = (dis->rows * col + row) - triangular(col + 1);
	}
	//printf("getDistance at index %d\n", idx);
	return dis->distances[idx];
}
void setDimenstions(distance* dis, int rows, int cols){

    dis->rows = rows;
    dis->cols = cols;
    int sub = (rows * rows -rows)/2;
    dis->distances = (double *)malloc(sub * sizeof(double));
    dis->coreDistances = (double *)malloc(dis->rows * sizeof(double));
}
void distance_compute(distance* dis, double* dataset, int rows, int cols, int numNeighbors){
	//printf("Calculating distance with numNeighbors = %d\n", numNeighbors);
	dis->numNeighbors = numNeighbors;

	/*dis->rows = rows;
	dis->cols = cols;

	int sub = (dis->rows * dis->rows - dis->rows) / 2;

	if(dis->distances != NULL){
		free(dis->distances);
	}

	dis->distances = malloc(sub * sizeof(double));

	if(dis->coreDistances != NULL){
		free(dis->coreDistances);
	}

	dis->coreDistances = malloc(dis->rows * sizeof(double));*/
	setDimenstions(dis, rows, cols);
	do_euclidean(dis, dataset);
}

