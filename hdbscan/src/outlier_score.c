/*
 * outlier_score.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include <outlier_score.h>

int create_outlier_score(outlier_score* os, double score, double coreDistance, int id){
	os = (outlier_score*)malloc(sizeof(outlier_score));

	if(os == NULL){
		return OUTLIERSCORE_ERROR;
	}

	os->coreDistance = coreDistance;
	os->id = id;
	os->score = score;

	return OUTLIERSCORE_SUCCESS;
}
void destroy_outlier_score(outlier_score* os){
	free(os);
}

