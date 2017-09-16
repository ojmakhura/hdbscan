/*
 * disctance.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include <stdlib.h>
#include <omp.h>

#include "hdbscan/distance.h"

int cmpint(const void * a, const void * b) {
	return (*(int*) a - *(int*) b);
}

double cmpdouble(const void * a, const void * b) {
	return (*(double*) a - *(double*) b);
}

/**
 *
 */
uint triangular(uint n) {
	return (n * n + n) / 2;
}

void distance_init(distance* dis, calculator cal, int minPoints) {
	if(dis == NULL){
		dis = malloc(sizeof(distance));
	}
	dis->cal = cal;
	dis->minPoints = minPoints;

}

/**
 *
 */
void distance_destroy(distance* d) {
	free(d->distances);
	free(d->coreDistances);
	free(d);
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

#ifdef USE_OPENMP
#pragma omp parallel
#endif
	{

		double sortedDistance[dis->rows];
#ifdef USE_OPENMP
#pragma omp for private (sortedDistance)
#endif
		for (uint i = 0; i < dis->rows; i++) {
			for (uint j = 0; j < dis->rows; j++) {
				double sum, diff = 0.0;
				uint offset2, offset1;
				sum = 0;

				double num1 = l2_norm(dataset + i, dis->cols);
				double num2 = l2_norm(dataset + j, dis->cols);

				diff = num1 - num2;
				sum += (diff * diff);
				sum = sqrt(sum);

				//printf("sum = %f\n", sum);
				int c;
				if (j > i) {
					// Calculate the linearised upper triangular matrix offset
					offset1 = i * dis->rows + j;
					c = offset1 - triangular(i + 1);
					//printf("c calculated %d from (%d, %d)\n", c, i, j);
					offset2 = j * dis->rows + i;
					//printf("offset calculated at (%d, %d)\n", offset1, offset2);

					*(distance + c) = sum;
					//printf("*(distance + c) 1 alloc %f\n", *(distance + c));
					//*(distance + offset2) = sum;
					//printf("distance sum set\n");
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
			dis->coreDistances[i] = sortedDistance[dis->minPoints];
		}
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

/*void distance_euclidean(double** dataset, distance* dis) {
	dis->rows = rows;
	dis->cols = cols;

	int sub = (dis->rows * dis->rows - dis->rows) / 2;

	if(dis->distances != NULL){
		free(dis->distances);
	}

	dis->distances = malloc(sub * sizeof(double));

	if(dis->coreDistances != NULL){
		free(dis->coreDistances);
	}

	dis->coreDistances = malloc(dis->rows * sizeof(double));
	do_euclidean(dataset, dis);
}*/

void distance_compute(distance* dis, double* dataset, int rows, int cols, int minPoints){

	dis->rows = rows;
	dis->cols = cols;

	int sub = (dis->rows * dis->rows - dis->rows) / 2;

	if(dis->distances != NULL){
		free(dis->distances);
	}

	dis->distances = malloc(sub * sizeof(double));

	if(dis->coreDistances != NULL){
		free(dis->coreDistances);
	}

	dis->coreDistances = malloc(dis->rows * sizeof(double));
	do_euclidean(dis, dataset);
}

