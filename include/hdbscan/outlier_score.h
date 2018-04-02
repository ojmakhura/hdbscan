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

#ifndef OUTLIER_SCORE_H_
#define OUTLIER_SCORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "listlib/utils.h"

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
