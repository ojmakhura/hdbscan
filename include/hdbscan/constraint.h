/*
 * constraint.h
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
/** @file constraint.h */
#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>


#define CONSTRAINT_SUCCESS 1
#define CONSTRAINT_ERROR 0


#ifdef __cplusplus
namespace clustering {
#endif

/**
 * \enum  CONSTRAINT_TYPE
 * 
 */
typedef enum _CONSTRAINT_TYPE {
	NULL_LINK, MUST_LINK, CANNOT_LINK
} CONSTRAINT_TYPE;

/*
#ifdef __cplusplus
char** MUST_LINK_TAG  = "ml";
char** CANNOT_LINK_TAG = "cl";
#endif
*/

/**
 * \struct Constraint
 * 
 * @brief A clustering constraint (either a must-link or cannot-link constraint between two points).
 * 
 * \typedef Constraint
 */
typedef struct Constraint{
	CONSTRAINT_TYPE type;
	int32_t pointA;
	int32_t pointB;

#ifdef __cplusplus
public:

	/**
	 * Creates a new constraint.
	 * @param pointA The first point involved in the constraint
	 * @param pointB The second point involved in the constraint
	 * @param type The CONSTRAINT_TYPE of the constraint
	 */
	Constraint(int pointA, int pointB, CONSTRAINT_TYPE type);
	virtual ~Constraint();
#endif


} constraint;

/**
 * @brief Create a new constraint and allocate all needed memory
 * 
 * @param c 
 * @param pointA 
 * @param pointB 
 * @param type 
 * @return constraint* 
 */
constraint* constraint_init(constraint* c, int pointA, int pointB, CONSTRAINT_TYPE type);

/**
 * @brief Free all dynamically allocate memory
 * 
 * @param cons 
 */
void constraint_destroy(constraint* cons);
#ifdef __cplusplus
};
}
#endif

#endif /* CONSTRAINT_H_ */
