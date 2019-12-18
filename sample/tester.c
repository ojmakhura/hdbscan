/*
 * tester.c
 *
 * A simple tester for the hdbscan library using C. This file shows how
 * call the different methods. It shows how to run the initial clustering
 * as well as how to recluster using the same data but different minPts.
 * There is examples of how to create the clustering table as well as
 * statistical values and the cluster confidences and sorting the
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

#include "hdbscan/hdbscan.h"
#include "hdbscan/logger.h"
#include "dataset.h"
#include "listlib/list.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

double* getDset(char* filename, index_t *rs, index_t *cs){

	ArrayList* list = (ArrayList *)array_list_init(10, sizeof(double), double_compare);

	FILE *fp = fopen(filename,"r");
	if(!fp){
		logger_write(FATAL, "Could not open file%s\n", filename);
		exit(0);
	}
	
	char buff[2048];
	char* delim = " ,\n\t\r\v";
	while(fgets(buff, 2048, fp) != 0) {
		char *toks = strtok (buff, delim);
		while(toks != NULL && !(strlen(toks) == 1 && isspace(toks[0]))){

			if(strlen(toks) > 0){
				double d = atof(toks);
				array_list_append(list, &d);
			}
			toks = strtok (NULL, delim);
		}

		(*rs)++;
	}
	*cs = (index_t)list->size/(*rs);
	double* dset = (double *) malloc(list->size * sizeof(double));
	double *ddata = (double *)list->data;
	for(size_t i = 0; i < list->size; i++){
		dset[i] = ddata[i];
	}
	array_list_delete(list);
	fclose(fp);
	return dset;
}

int main(int argc, char** argv){

	clock_t begin, end;
	int err;
	index_t rs = 0, cs = 0;
	double time_spent;
	logger_write(INFO, "argc = %d\n", argc);
	double* dset = NULL;
	char tmp[200];

	if(argc == 3){ // will read the dataset from the provided csv file
		dset = getDset(argv[2], &rs, &cs);
	} else { // will use the default dataset in dataset.h
		dset = dataset;
		rs = (index_t)rows;
		cs = (index_t)cols;
	}
	sprintf(tmp, "(rows, cols) = (%d, %d)\n", rs, cs);
	logger_write(INFO, tmp);
	hdbscan* scan = hdbscan_init(NULL, (index_t)atoi(argv[1]));
	boolean rerun_ = FALSE;
	if(scan == NULL){
		logger_write(FATAL, "Could not initialise hdbscan\n");
		exit(0);
	}
	logger_write(INFO, "SUCCESS: hdbscan fully initialised\n");

	// This lil loop demonstrates how to use the rerun function
	for(int i = 0; i < 15; i++){
		if(!rerun_){
			begin = clock();
			err = hdbscan_run(scan, dset, rs, cs, TRUE, H_DOUBLE);
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			
			sprintf(tmp, "hdbscan run Process took %f\n", time_spent);
			logger_write(INFO, tmp);
			rerun_ = TRUE;
		} else{
			begin = clock();
			hdbscan_rerun(scan, (index_t)(atoi(argv[1]) + i));
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			sprintf(tmp, "hdbscan rerun Process took %f\n", time_spent);
			logger_write(INFO, tmp);
		}
		
		logger_write(NONE, "%d ------- ***********************************************************************************\n");

		if(err == HDBSCAN_ERROR){
			logger_write(ERROR, "Could not run hdbscan\n");
		} else{
			logger_write(INFO, "SUCCESS: hdbscan clustering completed\n");
			IntIntListMap* clusterTable = hdbscan_create_cluster_map(scan->clusterLabels, 0, scan->numPoints);
			hdbscan_print_cluster_map(clusterTable);
			//hdbscan_print_hierarchies(scan->hierarchy, scan->numPoints, NULL);

			IntDistancesMap* dMap = hdbscan_get_min_max_distances(scan, clusterTable);
			clustering_stats stats;
			hdbscan_calculate_stats(dMap, &stats);
			//hdbscan_print_distance_map(dMap);
			
			ArrayList* sorted  = array_list_init(hashtable_size(clusterTable), sizeof(label_t), NULL);

			if(sizeof(label_t) == sizeof(int)) {
				sorted->compare = int_compare;
			} else if(sizeof(label_t) == sizeof(long)) {
				sorted->compare = long_compare;
			} else {
				sorted->compare = short_compare;
			}

			for(size_t i = 0; i < set_size(clusterTable->keys); i++){
				label_t k;
				set_value_at(clusterTable->keys, i, &k);
				array_list_append(sorted, &k);
			}

			sorted = hdbscan_sort_by_length(clusterTable, sorted);
			logger_write(NONE, "\n\nSorted by length = [\n");

			label_t *data = (label_t *)sorted->data;
			for(size_t i = 0; i < sorted->size; i++){
				ArrayList* l1;
				hashtable_lookup(clusterTable, data+i, &l1);
				logger_write(NONE, "%d : %ld\n", data[i], l1->size);
			}
			logger_write(NONE, "]\n\n");
			
			sorted = hdbscan_sort_by_similarity(dMap, sorted, INTRA_DISTANCE_TYPE); // There is choice to use CORE_DISTANCE_TYPE
			logger_write(NONE, "Sorted by similarity = [");

			data = (label_t *)sorted->data;
			for(size_t i = 0; i < sorted->size; i++){
				distance_values* dis;
				hashtable_lookup(dMap, data+i, &dis);
				logger_write(NONE, "%d : (%f, %f)\n", data[i], dis->dr_confidence, dis->cr_confidence);
			}
			logger_write(NONE, "]\n\n");
			array_list_delete(sorted);

			hdbscan_print_distance_map(dMap);
			hdbscan_print_stats(&stats);
			logger_write(INFO, "Clustering validity : %d\n", hdbscan_analyse_stats(&stats));
			
			/*logger_write("\n\nCluster labels = [");
			for(uint i = 0; i < scan->numPoints; i++){
				logger_write("%d ", scan->clusterLabels[i]);
			}
			logger_write("]\n\n");
			*/
			//hdbscan_print_outlier_scores(scan->outlierScores, scan->numPoints);
						
			hdbscan_destroy_distance_map(dMap);
			hdbscan_destroy_cluster_map(clusterTable);
		}

		logger_write(NONE, "***********************************************************************************\n\n");
		//break;
	}

	hdbscan_destroy(scan);

	return 0;
}
