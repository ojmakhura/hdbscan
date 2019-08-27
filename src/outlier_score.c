/*
 * outlier_score.c
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
 * @file outlier_score.c
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the outlier_score functions
 * 
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "hdbscan/outlier_score.h"

outlier_score* create_outlier_score(outlier_score* os, distance_t score, distance_t coreDistance, index_t id){
	if(os == NULL)
		os = (outlier_score*)malloc(sizeof(outlier_score));

	if(os == NULL){
		printf("OUTLIERSCORE_ERROR: Could not allocate memory for Outlier Score");
	} else{

		os->coreDistance = coreDistance;
		os->id = id;
		os->score = score;
	}
	return os;
}
void destroy_outlier_score(outlier_score* os){
	free(os);
}


int outlier_score_compare(const void* score1, const void* score2){
	
	outlier_score s1 = *(const outlier_score*)score1;
	outlier_score s2 = *(const outlier_score*)score2;
	
	if (s1.score > s2.score)
		return 1;
	else if (s1.score < s2.score)
		return -1;
	else {

		if (s1.coreDistance > s2.coreDistance)
			return 1;
		else if (s1.coreDistance < s2.coreDistance)
			return -1;
		else
			return s1.id - s2.id;
	}
}

