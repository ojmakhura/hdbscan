/*
 * constraint.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <string>

using namespace std;

namespace clustering {

enum CONSTRAINT_TYPE {
	NULL_LINK, MUST_LINK, CANNOT_LINK
};

const string MUST_LINK_TAG  = "ml";
const string CANNOT_LINK_TAG = "cl";

/**
 * A clustering constraint (either a must-link or cannot-link constraint between two points).
 * @author junior
 */
class Constraint {

private:
	CONSTRAINT_TYPE type;
	int pointA;
	int pointB;

public:

	/**
	 * Creates a new constraint.
	 * @param pointA The first point involved in the constraint
	 * @param pointB The second point involved in the constraint
	 * @param type The CONSTRAINT_TYPE of the constraint
	 */
	Constraint(int pointA, int pointB, CONSTRAINT_TYPE type);
	virtual ~Constraint();

	// ------------------------------ GETTERS & SETTERS ------------------------------

	int getPointA();

	int getPointB();

	CONSTRAINT_TYPE getType();
};

} /* namespace clustering */

#endif /* CONSTRAINT_H_ */
