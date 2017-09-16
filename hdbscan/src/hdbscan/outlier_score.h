/*
 * outlier_score.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

#include <stdlib.h>

#define OUTLIERSCORE_SUCCESS 1
#define OUTLIERSCORE_ERROR 0

/**
 * Simple storage class that keeps the outlier score, core distance, and id (index) for a single point.
 * OutlierScores are sorted in ascending order by outlier score, with core distances used to break
 * outlier score ties, and ids used to break core distance ties.
 * @author junior
 */

typedef struct OUTLIERSCORE{
	double score;
	double coreDistance;
	int id;
} outlier_score;

int outlier_score_init(outlier_score* os, double score, double coreDistance, int id);
void outlier_score_destroy(outlier_score* os);

#endif /* OUTLIER_SCORE_H_ */
