/*
 * constraint.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_
#include <stdlib.h>

#define CONSTRAINT_SUCCESS 1
#define CONSTRAINT_ERROR 0

typedef enum _CONSTRAINT_TYPE {
	NULL_LINK, MUST_LINK, CANNOT_LINK
} CONSTRAINT_TYPE;

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

} constraint;

int constraint_create(constraint* cons, int pointA, int pointB, CONSTRAINT_TYPE type);
void constraint_destroy(constraint* cons);


#endif /* CONSTRAINT_H_ */
