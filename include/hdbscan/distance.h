/*
 * distance.h
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

#ifndef DISTANCE_H_
#define DISTANCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <omp.h>
#include <math.h>
#include <stdint.h>
typedef unsigned int uint;

#define DISTANCE_SUCCESS 1
#define DISTANCE_ERROR 0

#ifdef __cplusplus
namespace clustering {
#endif

#ifndef COSINE
#define COSINE 			0
#endif

#ifndef _EUCLIDEAN
#define _EUCLIDEAN 		1
#endif

#define DATATYPE_FLOAT			0
#define DATATYPE_DOUBLE			1
#define DATATYPE_INT			2
#define DATATYPE_LONG			3
#define DATATYPE_SHORT			4

/**
 * Calculate the triangular number of n
 * 
 * @param n
 */ 
inline uint TRIANGULAR(uint n) {
	return (n * n + n) / 2;
}

//#define TRIANGULAR(n)((((n) * (n)) + (n)) / 2)

/*
#ifndef MANHATTAN
#define MANHATTAN		2
#endif
*/
/*

#ifndef PEARSON
#define PEARSON			3
#endif

#ifndef SUPREMUM
#define SUPREMUM		4
#endif
*/

typedef unsigned int calculator;

struct Distance{
	double* distances;
	double* coreDistances;
	uint rows, cols;
	uint internalRows, internalCols;
	int numNeighbors;
	calculator cal;
	uint datatype;

#ifdef __cplusplus
public:
	Distance(calculator cal);
	Distance();
	Distance(calculator cal, int32_t type);
	~Distance();

	/**
	 * Retrieves the distance from the array. Because the array
	 * is an upper triangular matrix stored as a 1 dimensional array
	 *
	 */
    double getDistance(uint row, uint col);
    void getCoreDistances(int32_t numNeighbors);

private:
    void setDimenstions(uint rows, uint cols);
	/**
	 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
	 */
	void doEuclidean(void* dataset, int32_t numNeighbors);

	void computeDistance(Distance* dis, void* dataset, int rows, int cols, int numNeighbors);
#endif
};


typedef struct Distance distance;

/**
 * 
 * 
 */ 
distance* distance_init(distance* dis, calculator cal, uint datatype);

/**
 * 
 */ 
void distance_destroy(distance* d);

/**
 * 
 * 
 */ 
void distance_clean(distance* d);

/**
 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
 */
void distance_euclidean(void* dataset, distance* dis, uint rows, uint cols);

/**
 * 
 * 
 */ 
double distance_get(distance* dis, uint row, uint col);

/**
 * 
 * 
 */ 
void distance_compute(distance* dis, void* dataset, int rows, int cols, int numNeighbors);

/**
 *
 *
 */
void distance_get_core_distances(distance *dis);

#ifdef __cplusplus
};
}
#endif
#endif /* DISTANCE_H_ */
