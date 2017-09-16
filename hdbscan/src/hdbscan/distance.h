/*
 * distance.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <omp.h>
#include <math.h>


#define DISTANCE_SUCCESS 1
#define DISTANCE_ERROR 0

#define COSINE 			0
#define _EUCLIDEAN 		1
#define MANHATTAN		2
#define PEARSON			3
#define SUPREMUM		4

typedef unsigned int calculator;

typedef struct _distance{
	double* distances;
	double* coreDistances;
	uint rows, cols;
	uint internalRows, internalCols;
	int minPoints;
	calculator cal;
} distance;

int distance_init(distance* distance, calculator cal, int minPoints);
void distance_destroy(distance* d);

/**
 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
 */
void distance_euclidean(double* dataset, distance* dis, uint rows, uint cols);

double distance_get(distance* dis, uint row, uint col);
void distance_compute(distance* dis, double* dataset, int rows, int cols, int minPoints);
#endif /* DISTANCE_H_ */
