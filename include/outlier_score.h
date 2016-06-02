/*
 * outlier_score.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

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
}outlier_score;

outlier_score* create_outlier_score(double score, double coreDistance, int id);
void destroy_outlier_score(outlier_score* os);

#endif /* OUTLIER_SCORE_H_ */
