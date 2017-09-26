/*
 * outlier_score.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include "hdbscan/outlier_score.h"

outlier_score* create_outlier_score(outlier_score* os, double score, double coreDistance, int id){
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

