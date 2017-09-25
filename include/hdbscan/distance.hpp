/*
 * distance.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <cmath>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

namespace clustering {
namespace distance {

#define COSINE 			0
#define _EUCLIDEAN 		1
#define _MANHATTAN		2
#define PEARSON			3
#define SUPREMUM		4

typedef unsigned int calculator;

template <class T>
class DistanceCalculator{

public:
	DistanceCalculator(calculator cal = _EUCLIDEAN);
	~DistanceCalculator();
	void computeDistance(T* dataset, int rows, int cols, int numNeighbors);
    void computeDistance(vector<T>& dataset, int rows, int cols, bool rowwise, int minPoints);
    void computeDistance(T* dataset, int rows, int cols, bool rowwise, int minPoints);

    double* getDistance();
    double* getCoreDistances();

	/**
	 * Retrieves the distance from the array. Because the array
	 * is an upper triangular matrix stored as a 1 dimensional array
	 *
	 */
    double getDistance(uint row, uint col);

	void setCalculator(calculator cal);
    void clean();

private:
	double* distance, *coreDistances;
	uint rows, cols;
	uint internalRows, internalCols;
	calculator cal = _EUCLIDEAN;

	/**
	 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
	 */
	void do_euclidean(T* dataset, int minPoints);

    void do_euclidean(void* dataset, int minPoints, bool rowwise, bool isVector);
	/**
	 * Computes cosine similarity between two points, d = 1 - ((X�Y) / (||X||*||Y||))
	 */
	void do_cosine(T* dataset, int minPoints);

	/**
	 * Computes the manhattan distance between two points, d = |x1-y1| + |x2-y2| + ... + |xn-yn|.
	 */
	void do_manhattan(T* dataset, int minPoints);

	/**
	 * Computes the euclidean distance between two points, d = 1 - (cov(X,Y) / (std_dev(X) * std_dev(Y)))
	 *
	 * @param dataset
	 * @param minPoints
	 */
	void do_pearson(T* dataset, int minPoints);

	/**
	 * Computes the supremum distance between two points, d = max[(x1-y1), (x2-y2), ... ,(xn-yn)].
	 */
	void do_supremum(T* dataset, int minPoints);

	/**
	 * Calculates the n_th triangular number
	 *
	 * @param n
	 *
	 */

	uint triangular(uint n);

	void addDistance(uint i, uint j, T distance);
    double getElement(void* dataset, int index, bool isVector);
    void setDimenstions(int rows, int cols);

};


}
}

#endif /* DISTANCE_H_ */
