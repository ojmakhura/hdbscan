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

/** 
 * @file distance.h 
 * 
 * @brief This header file declares the distance struct and functions.
 */
#ifndef DISTANCE_H_
#define DISTANCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include "hdbscan/utils.h"


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


typedef unsigned int calculator;

/**
 * \struct Distance
 * @brief The distance structure.
 */
struct Distance{
	double* distances;
	double* coreDistances;
	uint rows, cols;
	uint internalRows, internalCols;
	int numNeighbors;
	calculator cal;
	enum HTYPES datatype;

#ifdef __cplusplus
public:
	/**
	 * @brief Construct a new Distance object
	 * 
	 * @param cal 
	 */
	Distance(calculator cal);

	/**
	 * @brief Construct a new Distance object
	 * 
	 */
	Distance();

	/**
	 * @brief Construct a new Distance object
	 * 
	 * @param cal 
	 * @param type 
	 */
	Distance(calculator cal, enum HTYPES type);

	/**
	 * @brief Destroy the Distance object
	 * 
	 */
	~Distance();

	/**
	 * Retrieves the distance from the array. Because the array
	 * is an upper triangular matrix stored as a 1 dimensional array
	 *
	 */
    double getDistance(uint row, uint col);

	/**
	 * @brief Get the Core Distances object
	 * 
	 * @param numNeighbors 
	 */
    void getCoreDistances(int32_t numNeighbors);

private:
	/**
	 * @brief Set the Dimenstions object
	 * 
	 * @param rows 
	 * @param cols 
	 */
    void setDimenstions(uint rows, uint cols);
	
	/**
	 * @brief C++ version of distance_compute
	 * 
	 * @param dis 
	 * @param dataset 
	 * @param rows 
	 * @param cols 
	 * @param numNeighbors 
	 */
	void computeDistance(Distance* dis, void* dataset, int rows, int cols, int numNeighbors);
#endif
};

typedef struct Distance distance; /**\typedef distance */

/**
 * @brief 
 * 
 * @param dis 
 * @param cal 
 * @param datatype 
 * @return distance* 
 */
distance* distance_init(distance* dis, calculator cal, enum HTYPES datatype);

/**
 * @brief Free all distance dynamic memory including emod allocated for d
 * 
 * @param d Distance object
 */
void distance_destroy(distance* d);

/**
 * @brief Clean internal dynamic memory leaving d.
 * 
 * @param d 
 */
void distance_clean(distance* d);

/**
 * @brief Get the distance between row and col
 * 
 * It should be noted that the distace matrix is only storing the lower half
 * of the distance matrix. See distance_compute() for more information. 
 * 
 * @param dis 
 * @param row 
 * @param col 
 * @return double 
 */
double distance_get(distance* dis, uint row, uint col);

/**
 * @brief Computes the euclidean distance between two points, 
 * 
 * The distance is calculated as d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2). 
 * The function takes advantage of the fact that when calculating distance between
 * dataset entries, the distances above the principal diagonal are reflected about
 * the diagonal with the diagonal itself having 0. As such we can reduce memory
 * and computational costs by only calculating (rows * rows -rows)/2 values instead
 * of rows * rows.
 * 
 * @param dis Distance object
 * @param dataset The dataset
 * @param rows number of rows
 * @param cols numer of columns
 * @param numNeighbors minimum number of neighbours
 */
void distance_compute(distance* dis, void* dataset, int rows, int cols, int numNeighbors);

/**
 * @brief Find the core distances based on the number of neighbours
 * 
 * @param dis 
 */
void distance_get_core_distances(distance *dis);

#ifdef __cplusplus
};
}
#endif
#endif /* DISTANCE_H_ */