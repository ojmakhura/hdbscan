/*
 * tester.c
 *
 *  Created on: 17 Sep 2017
 *      Author: junior
 */

//#include "config.h"
#include "hdbscan/hdbscan.h"
#include "dataset.h"
#include <time.h>

int main(int argc, char** argv){
	clock_t begin, end;
	int err;
	double time_spent;
	hdbscan* scan = hdbscan_init(NULL, atoi(argv[1]), DATATYPE_DOUBLE);
	if(scan == NULL){
		printf("ERROR: Could not initialise hdbscan\n");
		exit(0);
	} else {
		printf("SUCCESS: hdbscan fully initialised\n");
		begin = clock();
		err = hdbscan_run(scan, dataset, rows, cols, TRUE);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("hdbscan run Process took %f\n", time_spent);
	}
	
	for(int i = 0; i < 10; i++){
		printf("%d ------- ***********************************************************************************\n", scan->minPoints);
		
		if(err == HDBSCAN_ERROR){
			printf("ERROR: Could not run hdbscan\n");
		} else{
			printf("SUCCESS: hdbscan clustring completed\n");
			//printf("Number total number of clusters is %d\n\n", scan->clusters->len);
			IntIntListMap* clusterTable = hdbscan_create_cluster_table(scan->clusterLabels, 0, scan->numPoints);
			hdbscan_print_cluster_table(clusterTable);
				
			IntDoubleListMap* dMap = hdbscan_get_min_max_distances(scan, clusterTable);
			hdbscan_print_distance_map_table(dMap);
			StringDoubleMap* stats = hdbscan_calculate_stats(dMap);
			hdbscan_print_stats_map(stats);
								
			printf("Clustering validity : %d\n", hdbscan_analyse_stats(stats));
				
			hdbscan_destroy_distance_map_table(dMap);
			hdbscan_destroy_stats_map(stats);
	
			printf("\n\nCluster labels = [");
			for(uint i = 0; i < scan->numPoints; i++){
				printf("%d ", scan->clusterLabels[i]);
			}
			printf("]\n\n");
		}
		
		hdbscan_rerun(scan, atoi(argv[1]) + i + 1);		
		printf("***********************************************************************************\n\n");
		//exit(0);
	}
	hdbscan_clean(scan);

	return 0;
}
