/*
 * distance.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <cmath>
#include <vector>
#include <cstddef>

using namespace std;

#define COSINE 			0
#define _EUCLIDEAN 		1
#define _MANHATTAN		2
#define PEARSON			3
#define SUPREMUM		4

namespace clustering {
namespace distance {

typedef unsigned int calculator;

class DistanceCalculator{

public:
DistanceCalculator(calculator cal);
double computeDistance(vector<double>* attributesOne,
		vector<double>* attributesTwo);

private:
	vector<double>* attributesOne;
	vector<double>* attributesTwo;
	double distance;
	calculator cal = _EUCLIDEAN;

	/**
	 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
	 * @author junior
	 */
	void do_euclidean();

	/**
	 * Computes cosine similarity between two points, d = 1 - ((X�Y) / (||X||*||Y||))
	 * @author junior
	 */
	void do_cosine();

	/**
	 * Computes the manhattan distance between two points, d = |x1-y1| + |x2-y2| + ... + |xn-yn|.
	 * @author junior
	 */
	void do_manhattan();

	/**
	 * Computes the euclidean distance between two points, d = 1 - (cov(X,Y) / (std_dev(X) * std_dev(Y)))
	 * @author junior
	 */
	void do_pearson();

	/**
	 * Computes the supremum distance between two points, d = max[(x1-y1), (x2-y2), ... ,(xn-yn)].
	 * @author junior
	 */
	void do_supremum();

};
}
}

#endif /* DISTANCE_H_ */
