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

	for(int i = 0; i < 1000; i++){
		printf("***********************************************************************************\n");
		//clock_t begin = clock();
		hdbscan* scan = hdbscan_init(NULL, atoi(argv[1]), DATATYPE_DOUBLE);

		if(scan == NULL){
			printf("ERROR: Could not initialise hdbscan\n");
		} else{
			printf("SUCCESS: hdbscan fully initialised\n");
			clock_t begin = clock();
			int err = hdbscan_run(scan, dataset, rows, cols, TRUE);
			clock_t end = clock();
			double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("hdbscan run Process took %f\n", time_spent);
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
		}
		
		
		
		hdbscan_clean(scan);
		printf("***********************************************************************************\n\n");
		exit(0);
	}

	return 0;
}
