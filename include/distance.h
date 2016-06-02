/*
 * distance.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef DISTANCE_H_
#define DISTANCE_H_

#define COSINE 			0
#define EUCLIDEAN 		1
#define MANHATTAN		2
#define PEARSON			3
#define SUPREMUM		4

typedef unsigned int calculator;

double computeDistance(double* attributesOne, double* attributesTwo, calculator cal);

/**
 * Computes the euclidean distance between two points, d = sqrt((x1-y1)^2 + (x2-y2)^2 + ... + (xn-yn)^2).
 * @author junior
 */
void do_euclidean(double* attributesOne, double* attributesTwo, size_t s);

/**
 * Computes cosine similarity between two points, d = 1 - ((X�Y) / (||X||*||Y||))
 * @author junior
 */
void do_cosine(double* attributesOne, double* attributesTwo, size_t s);

/**
 * Computes the manhattan distance between two points, d = |x1-y1| + |x2-y2| + ... + |xn-yn|.
 * @author junior
 */
void do_manhattan(double* attributesOne, double* attributesTwo, size_t s);

/**
 * Computes the euclidean distance between two points, d = 1 - (cov(X,Y) / (std_dev(X) * std_dev(Y)))
 * @author junior
 */
void do_pearson(double* attributesOne, double* attributesTwo, size_t s);

/**
 * Computes the supremum distance between two points, d = max[(x1-y1), (x2-y2), ... ,(xn-yn)].
 * @author junior
 */
void do_supremum(double* attributesOne, double* attributesTwo, size_t s);

#endif /* DISTANCE_H_ */
