/*
 * constraint.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "constraint.hpp"

namespace clustering {

Constraint::Constraint(int pointA, int pointB, CONSTRAINT_TYPE type) {
	this->pointA = pointA;
	this->pointB = pointB;
	this->type = type;
}

Constraint::~Constraint() {

}

// ------------------------------ GETTERS & SETTERS ------------------------------

int Constraint::getPointA() {
	return this->pointA;
}

int Constraint::getPointB() {
	return this->pointB;
}

CONSTRAINT_TYPE Constraint::getType() {
	return this->type;
}

} /* namespace clustering */
