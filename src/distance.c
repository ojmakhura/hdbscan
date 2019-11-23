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

/**
 * @file distance.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the distance calculations for HDBSCAN
 * 
 * @version 3.1.6
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "hdbscan/distance.h"

#ifdef _OPENMP
#include <omp.h>
#endif


/**
 * @brief Initialise the struct. We set the get_diff function based on the
 * datatype.
 * 
 * @param dis 
 * @param cal 
 * @param datatype 
 * @return distance* 
 */
distance* distance_init(distance* dis, calculator cal, enum HTYPES datatype) {
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

distance_t distance_get(distance* dis, index_t row, index_t col) {
	size_t idx;
	if (row < col) {
		idx = (dis->rows * row + col) - TRIANGULAR_H(row + 1);

	} else if (row == col) {
		return 0;
	} else {
		idx = (dis->rows * col + row) - TRIANGULAR_H(col + 1);
	}
	return dis->distances[idx];
}

/**
 * @brief Compute the euclidean distance. We also calculate the size 
 * of the distance matrix using (rows * rows -rows)/2
 * 
 * @param dis 
 * @param dataset 
 * @param rows 
 * @param cols 
 * @param numNeighbors 
 */
void distance_compute(distance* dis, void* dataset, index_t rows, index_t cols, index_t numNeighbors){
	dis->numNeighbors = numNeighbors;
	//setDimenstions(dis, rows, cols);
	dis->rows = rows;
    dis->cols = cols;
    size_t sub = (rows * rows -rows)/2;
    dis->distances = (distance_t *)malloc(sub * sizeof(distance_t));
    dis->coreDistances = (distance_t *)malloc(dis->rows * sizeof(distance_t));
	distance_t sum, diff;

#ifdef _OPENMP
#pragma omp parallel for private(sum, diff)   /// Use omp to speed up calculations
#endif
	for (size_t i = 0; i < dis->rows; i++) {
		for (size_t j = i + 1; j < dis->rows; j++) {
			diff = 0.0;
			sum = 0;

            for (size_t k = 0; ((k < dis->cols) && (i != j)); k++) {
				if(dis->datatype == H_DOUBLE) {

					double* dt = dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);

				} else if(dis->datatype == H_FLOAT) {

					float* dt = dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);

				} else if(dis->datatype == H_INT) {

					int* dt = dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);
				} else if(dis->datatype == H_LONG) {

					long* dt = dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);
				} else if(dis->datatype == H_SHORT) {

					short* dt = dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);
				} else {

					char* dt = (char*)dataset;
					diff = (distance_t)(dt[i * dis->cols + k] - dt[j * dis->cols + k]);
				}

    			//diff = get_diff(dis, dataset, i, j, k);
    			sum += (diff * diff);
            }

			sum = sqrt(sum);

			// Calculate the linearised upper triangular matrix offset
			size_t offset = i * dis->rows + j;
			size_t c = offset - TRIANGULAR_H((uint)i + 1);
			//printf("%ld: c = %ld\n", (rows * rows -rows)/2, c);			
			dis->distances[c] = sum;
			
		}
	}
	distance_get_core_distances(dis);
	//exit(0);
}

/**
 * @brief Get the core distance from the distance array
 * 
 * There is no point in saving the distances for the entire row
 * then filtering since we only want the first dis->numNeighbors 
 * distances. As such we keep a sorted array of those first elements.
 * For every point, we get the distance and insert it into the proper
 * place in the array, discarding the last distance in the array.
 * 
 * By keeping the array size to dis->numNeighbors, we ensure we are
 * only ever sorting the needed portion of the distances. 
 * 
 * @param dis 
 */
void distance_get_core_distances(distance *dis)
{
	
	distance_t sortedDistance[dis->numNeighbors+1];
#ifdef _OPENMP	
#pragma omp parallel for private(sortedDistance)
#endif
	for (index_t i = 0; i < dis->rows; i++) {

		/// Fill sortedDistance with the largest possible value of distance_t
		for (index_t j = 0; j < dis->numNeighbors+1; j++) {
			sortedDistance[j] = D_MAX;
		}

		for (index_t j = 0; j < dis->rows; j++) {
			distance_t t = distance_get(dis, i, j);
			
			index_t low = 0;
			index_t high = dis->numNeighbors+1;

			// No need to attempt insertion if the distance is already
			// greater than the last entry in sortedDistance
			if(t > sortedDistance[dis->numNeighbors])
				continue;

			// Look for the entry position
			do {
				index_t mid = low + (high - low) / 2;
				
				if (sortedDistance[mid] > t) {
					high = mid;
				} else if (sortedDistance[mid] == t) {
					break;
				} else {
					low = mid + 1;
				}
				
			} while(low < high);

			index_t s = dis->numNeighbors+1;
			if((low < s) && (sortedDistance[ dis->numNeighbors] != t)) {
				size_t ds = sizeof(distance_t);
				size_t t3 = (s - low - 1) * ds; 	// Number of bytes to copy

				/* Shift the data to the right */
				memmove(sortedDistance + low + 1, sortedDistance + low, t3);

				// Add the data at the appropriate location
				sortedDistance[low] = t;
			}
		}
		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];
	}

}

void distances_print(distance *dis) {
	
	for (index_t i = 0; i < dis->rows; i++) {
		printf("[");
		for (index_t j = i + 1; j < dis->rows; j++) {
			printf("%f ", distance_get(dis, i, j));
		}
		printf("]\n");
	}
}