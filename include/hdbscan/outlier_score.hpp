/*
 * outlier_score.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

namespace clustering {

/**
 * Simple storage class that keeps the outlier score, core distance, and id (index) for a single point.
 * OutlierScores are sorted in ascending order by outlier score, with core distances used to break
 * outlier score ties, and ids used to break core distance ties.
 * @author junior
 */
class OutlierScore {

private:
	float score;
	float coreDistance;
	int id;

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

	bool operator<(const OutlierScore& other);
	int compareTO(const OutlierScore& other);

	// ------------------------------ GETTERS & SETTERS ------------------------------

	float getScore();
	float getCoreDistance();
	int getId();
};

} /* namespace clustering */

#endif /* OUTLIER_SCORE_H_ */
