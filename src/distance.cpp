/*
 * distance.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "distance.hpp"

using namespace std;

namespace clustering {
namespace distance {

DistanceCalculator::DistanceCalculator(calculator cal){
	this->cal = cal;
	distance = 0;

	this->attributesOne = NULL;
	this->attributesTwo = NULL;
}

double DistanceCalculator::computeDistance(vector<double>* attributesOne,
		vector<double>* attributesTwo) {

	distance = 0;

	this->attributesOne = attributesOne;
	this->attributesTwo = attributesTwo;

	if (cal == EUCLIDEAN) {
		do_euclidean();
	} else if (cal == COSINE) {
		do_cosine();
	} else if (cal == MANHATTAN) {
		do_manhattan();
	} else if (cal == PEARSON) {
		do_pearson();
	} else if (cal == SUPREMUM) {
		do_supremum();
	}

	return distance;
}

void DistanceCalculator::do_euclidean() {
	distance = 0;
	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		distance += (((*attributesOne)[i] - (*attributesTwo)[i])
				* ((*attributesOne)[i] - (*attributesTwo)[i]));
	}

	distance = sqrt(distance);
}

void DistanceCalculator::do_cosine() {
	double dotProduct = 0;
	double magnitudeOne = 0;
	double magnitudeTwo = 0;

	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		dotProduct += ((*attributesOne)[i] * (*attributesTwo)[i]);
		magnitudeOne += ((*attributesOne)[i] * (*attributesOne)[i]);
		magnitudeTwo += ((*attributesTwo)[i] * (*attributesTwo)[i]);
	}

	distance = 1 - (dotProduct / sqrt(magnitudeOne * magnitudeTwo));

}

void DistanceCalculator::do_manhattan() {
	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		distance += abs((*attributesOne)[i] - (*attributesTwo)[i]);
	}
}

void DistanceCalculator::do_pearson() {
	double meanOne = 0;
	double meanTwo = 0;

	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		meanOne += (*attributesOne)[i];
		meanTwo += (*attributesTwo)[i];
	}

	meanOne = meanOne / attributesOne->size();
	meanTwo = meanTwo / attributesTwo->size();

	double covariance = 0;
	double standardDeviationOne = 0;
	double standardDeviationTwo = 0;

	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		covariance += (((*attributesOne)[i] - meanOne)
				* ((*attributesTwo)[i] - meanTwo));
		standardDeviationOne += (((*attributesOne)[i] - meanOne)
				* ((*attributesOne)[i] - meanOne));
		standardDeviationTwo += (((*attributesTwo)[i] - meanTwo)
				* ((*attributesTwo)[i] - meanTwo));
	}

	distance = (1
			- (covariance / sqrt(standardDeviationOne * standardDeviationTwo)));
}

void DistanceCalculator::do_supremum() {
	for (unsigned int i = 0; i < attributesOne->size() && i < attributesTwo->size(); i++) {
		double difference = abs((*attributesOne)[i] - (*attributesTwo)[i]);
		if (difference > distance)
			distance = difference;
	}
}

}
}
