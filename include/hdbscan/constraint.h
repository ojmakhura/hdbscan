/*
 * constraint.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

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
 * A clustering constraint (either a must-link or cannot-link constraint between two points).
 * @author junior
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

constraint* constraint_init(constraint* c, int pointA, int pointB, CONSTRAINT_TYPE type);
void constraint_destroy(constraint* cons);
#ifdef __cplusplus
};
}
#endif

#endif /* CONSTRAINT_H_ */
