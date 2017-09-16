/*
 * constraint.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */

#include "hdbscan/constraint.h"

int constraint_create(constraint* c, int pointA, int pointB, CONSTRAINT_TYPE type){
	c = (constraint*)malloc(sizeof(constraint));

	if(c == NULL){
		return CONSTRAINT_ERROR;
	}

	c->pointA = pointA;
	c->pointB = pointB;
	c->type = type;

	return CONSTRAINT_SUCCESS;
}

void constraint_destroy(constraint* c){
	if(c != NULL)
		free(c);
}

