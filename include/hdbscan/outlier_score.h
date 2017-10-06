/*
 * outlier_score.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../containers/utils.h"

#define OUTLIERSCORE_SUCCESS 1
#define OUTLIERSCORE_ERROR 0

#ifdef __cplusplus
namespace clustering {
#endif
/**
 * Simple storage class that keeps the outlier score, core distance, and id (index) for a single point.
 * OutlierScores are sorted in ascending order by outlier score, with core distances used to break
 * outlier score ties, and ids used to break core distance ties.
 * @author junior
 */

struct OutlierScore{
	double score;
	double coreDistance;
	int32_t id;

#ifdef __cplusplus
public:
	/**
	 * Creates a new OutlierScore for a given point.
	 * @param score The outlier score of the point
	 * @param coreDistance The point's core distance
	 * @param id The id (index) of the point
	 */
	OutlierScore(float score, float coreDistance, int id);
	OutlierScore();
	virtual ~OutlierScore();

	// ------------------------------ PUBLIC METHODS ------------------------------

	boolean operator<(const OutlierScore& other);
	int compareTO(const OutlierScore& other);
#endif
};

typedef struct OutlierScore outlier_score;

outlier_score* outlier_score_init(outlier_score* os, double score, double coreDistance, int32_t id);
void outlier_score_destroy(outlier_score* os);


int outlier_score_compare(const void* score1, const void* score2);

#ifdef __cplusplus

}; /* namespace clustering */
}
#endif

#endif /* OUTLIER_SCORE_H_ */
