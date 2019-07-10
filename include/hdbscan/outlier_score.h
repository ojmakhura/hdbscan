/*
 * outlier_score.h
 *
 *  Copyright 2018 Onalenna Junior Makhura
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

/** @file outlier_score.h */
#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hdbscan/utils.h"

#define OUTLIERSCORE_SUCCESS 1
#define OUTLIERSCORE_ERROR 0

#ifdef __cplusplus
namespace clustering {
#endif
/**
 * \struct OutlierScore
 * @brief Simple storage class that keeps the outlier score, core distance, and id (index) for a single point.
 * 
 * OutlierScores are sorted in ascending order by outlier score, with core distances used to break
 * outlier score ties, and ids used to break core distance ties.
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

	/**
	 * @brief Construct a new Outlier Score object
	 * 
	 */
	OutlierScore();

	/**
	 * @brief Destroy the Outlier Score object
	 * 
	 */
	virtual ~OutlierScore();

	// ------------------------------ PUBLIC METHODS ------------------------------
	/**
	 * @brief 
	 * 
	 * @param other 
	 * @return boolean 
	 */
	boolean operator<(const OutlierScore& other);

	/**
	 * @brief 
	 * 
	 * @param other 
	 * @return int 
	 */
	int compareTO(const OutlierScore& other);
#endif
};

/** \typedef outlier_score */
typedef struct OutlierScore outlier_score;

/**
 * @brief 
 * 
 * @param os 
 * @param score 
 * @param coreDistance 
 * @param id 
 * @return outlier_score* 
 */
outlier_score* outlier_score_init(outlier_score* os, double score, double coreDistance, int32_t id);

/**
 * @brief 
 * 
 * @param os 
 */
void outlier_score_destroy(outlier_score* os);

/**
 * @brief 
 * 
 * @param score1 
 * @param score2 
 * @return int 
 */
int outlier_score_compare(const void* score1, const void* score2);

#ifdef __cplusplus

}; /* namespace clustering */
}
#endif

#endif /* OUTLIER_SCORE_H_ */
