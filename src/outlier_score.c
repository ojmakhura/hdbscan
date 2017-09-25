/*
 * outlier_score.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include "hdbscan/outlier_score.h"

outlier_score* create_outlier_score(double score, double coreDistance, int id){
	outlier_score* os = (outlier_score*)malloc(sizeof(outlier_score));

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

