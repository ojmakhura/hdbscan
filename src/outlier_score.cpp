/*
 * outlier_score.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "hdbscan/outlier_score.hpp"

namespace clustering {

OutlierScore::OutlierScore(float score, float coreDistance, int id) {
	this->score = score;
	this->coreDistance = coreDistance;
	this->id = id;
}
OutlierScore::OutlierScore(){
	id = -1;
	score = -1;
	coreDistance = -1;
}

OutlierScore::~OutlierScore() {
	// TODO Auto-generated destructor stub
}

// ------------------------------ PUBLIC METHODS ------------------------------

bool OutlierScore::operator<(const OutlierScore& other) {

	if(this->score < other.score && this->coreDistance < other.coreDistance){
		return true;
	} else{
		return false;
	}
}

int OutlierScore::compareTO(const OutlierScore& other){
	if (this->score > other.score)
		return 1;
	else if (this->score < other.score)
		return -1;
	else {

		if (this->coreDistance > other.coreDistance)
			return 1;
		else if (this->coreDistance < other.coreDistance)
			return -1;
		else
			return this->id-other.id;
	}
}

// ------------------------------ PRIVATE METHODS ------------------------------

// ------------------------------ GETTERS & SETTERS ------------------------------

float OutlierScore::getScore() {
	return this->score;
}

float OutlierScore::getCoreDistance(){
	return this->coreDistance;
}

int OutlierScore::getId() {
	return this->id;
}
} /* namespace clustering */
