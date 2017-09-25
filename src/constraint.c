/*
 * constraint.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */

#include "hdbscan/constraint.h"
#include <stdio.h>

constraint* constraint_create(constraint* c, int pointA, int pointB, CONSTRAINT_TYPE type){
	if(c == NULL)
		c = (constraint*)malloc(sizeof(constraint));

	if(c == NULL){
		printf("CONSTRAINT ERROR: Could not allocate memory for constraint\n");
	} else{

		c->pointA = pointA;
		c->pointB = pointB;
		c->type = type;
	}
	return c;
}

void constraint_destroy(constraint* c){
	if(c != NULL)
		free(c);
}

