/*
 * constraint.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

enum CONSTRAINT_TYPE {
	NULL_LINK, MUST_LINK, CANNOT_LINK
};

const char** MUST_LINK_TAG  = "ml";
const char** CANNOT_LINK_TAG = "cl";

/**
 * A clustering constraint (either a must-link or cannot-link constraint between two points).
 * @author junior
 */

typedef struct CONSTRAINT{
	CONSTRAINT_TYPE type;
	int pointA;
	int pointB;

}constraint;

constraint* create_constraint(int pointA, int pointB, CONSTRAINT_TYPE type);
void constraint_cleanup(constraint* cons);


#endif /* CONSTRAINT_H_ */
