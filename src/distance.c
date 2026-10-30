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
#include "hdbscan/logger.h"

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
		logger_write(FATAL, "distance_init - Failed to allocate memory for distance");
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
 * @brief Free the memory allocated for the distance struct.
 * 
 * @param d 
 */
void distance_destroy(distance* d) {
	distance_clean(d);
	if(d != NULL)
		free(d);
}

/**
 * @brief Clean up memory allocation by freeing the distances and coreDIstance memory.
 * 
 * @param d 
 */
void distance_clean(distance* d){
	if(d == NULL)
		return;

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
 * \brief Get the distance between the elements row and col. This function used the 
 * TRIANGULAR_H function to find the map the rwo and col values into the truncated 
 * distance vector.
 * 
 * @param dis 
 * @param row 
 * @param col 
 * @return distance_t 
 */
distance_t distance_get(distance* dis, index_t row, index_t col) {
	size_t idx;
	if (row < col) {
		idx = (size_t)dis->rows * row + col - TRIANGULAR_H((size_t)row + 1);

	} else if (row == col) {
		return 0;
	} else {
		idx = (size_t)dis->rows * col + row - TRIANGULAR_H((size_t)col + 1);
	}
	return dis->distances[idx];
}

#ifdef _OPENMP
/// Rows near the top of the triangle have the most work, so hand them out dynamically
#define DISTANCE_OMP_FOR _Pragma("omp parallel for schedule(dynamic, 16)")
#else
#define DISTANCE_OMP_FOR
#endif

/**
 * @brief Define a euclidean distance kernel for one input datatype. Each
 * datatype gets its own copy of the loop so that the type check is not done
 * for every element.
 */
#define DEFINE_DISTANCE_COMPUTE(NAME, TYPE)											\
static void NAME(distance* dis, const void* dataset) {								\
	const TYPE* dt = (const TYPE*)dataset;											\
	size_t n = dis->rows;															\
	size_t cols = dis->cols;														\
	DISTANCE_OMP_FOR																\
	for (size_t i = 0; i < n; i++) {												\
		const TYPE* a = dt + i * cols;												\
		/* Offset of row i in the linearised upper triangular matrix. The		\
		 * subtraction may wrap for i = 0, but base + j is always in range. */	\
		size_t base = i * n - TRIANGULAR_H(i + 1);									\
		for (size_t j = i + 1; j < n; j++) {										\
			const TYPE* b = dt + j * cols;											\
			distance_t sum = 0;														\
			for (size_t k = 0; k < cols; k++) {										\
				distance_t diff = (distance_t)(a[k] - b[k]);						\
				sum += (diff * diff);												\
			}																		\
			dis->distances[base + j] = (distance_t)sqrt(sum);						\
		}																			\
	}																				\
}

DEFINE_DISTANCE_COMPUTE(distance_compute_double, double)
DEFINE_DISTANCE_COMPUTE(distance_compute_float, float)
DEFINE_DISTANCE_COMPUTE(distance_compute_int, int)
DEFINE_DISTANCE_COMPUTE(distance_compute_long, long)
DEFINE_DISTANCE_COMPUTE(distance_compute_short, short)
DEFINE_DISTANCE_COMPUTE(distance_compute_char, char)

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
	
	dis->rows = rows;
    dis->cols = cols;
	// Compute the triangular matrix size in size_t so that large datasets do not
	// overflow index_t, and make sure the byte count itself does not overflow.
	size_t n = (size_t)rows;
	if(n > 1 && (n - 1 > SIZE_MAX / n || (n * (n - 1)) / 2 > SIZE_MAX / sizeof(distance_t))) {
		logger_write(FATAL, "distance_compute - Dataset too large for the distance matrix");
		return;
	}
	size_t sub = n > 1 ? (n * (n - 1)) / 2 : 0;

	dis->distances = (distance_t *)malloc((sub > 0 ? sub : 1) * sizeof(distance_t));
	dis->coreDistances = (distance_t *)malloc((n > 0 ? n : 1) * sizeof(distance_t));

	if(dis->distances == NULL || dis->coreDistances == NULL) {
		logger_write(FATAL, "distance_compute - Failed to allocate memory for distances");
		distance_clean(dis);
		return;
	}
	/// Branch on the datatype once instead of inside the innermost loop
	if(dis->datatype == H_DOUBLE) {
		distance_compute_double(dis, dataset);
	} else if(dis->datatype == H_FLOAT) {
		distance_compute_float(dis, dataset);
	} else if(dis->datatype == H_INT) {
		distance_compute_int(dis, dataset);
	} else if(dis->datatype == H_LONG) {
		distance_compute_long(dis, dataset);
	} else if(dis->datatype == H_SHORT) {
		distance_compute_short(dis, dataset);
	} else {
		distance_compute_char(dis, dataset);
	}
	distance_get_core_distances(dis);
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

		// Index the triangular matrix directly rather than calling distance_get
		// for every pair. The row base may wrap for i = 0; base + j stays in range.
		size_t n = dis->rows;
		size_t base = (size_t)i * n - TRIANGULAR_H((size_t)i + 1);
		for (index_t j = 0; j < dis->rows; j++) {
			distance_t t;
			if (j < i) {
				t = dis->distances[(size_t)j * n + i - TRIANGULAR_H((size_t)j + 1)];
			} else if (j == i) {
				t = 0;
			} else {
				t = dis->distances[base + j];
			}

			index_t s = (index_t)(dis->numNeighbors + 1);
			index_t low = 0;
			index_t high = s;

			// No need to attempt insertion if the distance is not smaller
			// than the last entry in sortedDistance
			if(t >= sortedDistance[dis->numNeighbors])
				continue;

			// Find the first entry greater than t (upper bound) so that ties
			// keep the array sorted. Stopping at an equal entry would leave
			// low pointing anywhere before it.
			while(low < high) {
				index_t mid = (index_t)(low + (high - low) / (index_t)2);

				if (sortedDistance[mid] > t) {
					high = mid;
				} else {
					low = (index_t)(mid + 1);
				}
			}

			if(low < s) {
				size_t ds = sizeof(distance_t);
				size_t t3 = (size_t)((size_t)(s - low - 1) * ds); 	// Number of bytes to copy

				/* Shift the data to the right */
				memmove(sortedDistance + low + 1, sortedDistance + low, t3);

				// Add the data at the appropriate location
				sortedDistance[low] = t;
			}
		}
		dis->coreDistances[i] = sortedDistance[dis->numNeighbors];
	}

}

/**
 * @brief Print the distane values.
 * 
 * @param dis 
 */
void distances_print(distance *dis) {
	
	char s[20];
	for (index_t i = 0; i < dis->rows; i++) {
		logger_write(NONE, "[");

		for (index_t j = (index_t)(i + 1); j < dis->rows; j++) {
			snprintf(s, sizeof(s), "%f ", distance_get(dis, i, j));
			logger_write(NONE, s);
		}

		logger_write(NONE, "]\n");
	}
}