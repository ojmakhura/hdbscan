/*
 * constraint.c
 *
 * Copyright 2018 Onalenna Junior Makhura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file constraint.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the functions in constraint.h
 * 
 * @date 2019-07-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "hdbscan/constraint.h"
#include <stdio.h>
#ifdef DEBUG
#include "hdbscan/logger.h"
#endif

constraint* constraint_create(constraint* c, int pointA, int pointB, CONSTRAINT_TYPE type){
	if(c == NULL)
		c = (constraint*)malloc(sizeof(constraint));

	if(c == NULL){
	#ifdef DEBUG
		logger_write(FATAL, "constraint_create - Could not allocate memory for constraint\n");
	#else
		printf("FATAL: constraint_create - Could not allocate memory for constraint\n");
	#endif
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

