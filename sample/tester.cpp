/*
 * ghdbscan.cpp
 *
 *  Created on: 7 Oct 2016
 *      Author: ojmakh
 */
//#include "config.h"
#include "hdbscan/hdbscan.hpp"
#include "dataset.h"

using namespace std;
using namespace clustering;

void dummy_tester(int minPts){
	hdbscan scan(minPts, DATATYPE_DOUBLE);
	scan.run(dataset, rows, cols, TRUE);
	bool rerun = false;
	for(int i = 0; i < 10; i++){
		printf("%d ------- ***********************************************************************************\n", scan.minPoints);
		
		if(rerun){
			scan.reRun(minPts + i);
		}
		
		map_t clusterTable = createClusterTable(scan.clusterLabels, 0, scan.numPoints);		
		printClusterTable(clusterTable);
		
		map_d dMap = getMinMaxDistances(scan, clusterTable);
		printDistanceMapTable(dMap);
		
		map<string, double> stats = calculateStats(dMap);
		printStatsMap(stats);
		
		printf("Clustering validity : %d\n", analyseStats(stats));
				
		printf("\n\nCluster labels = [");
		for(uint i = 0; i < scan.numPoints; i++){
			printf("%d ", scan.clusterLabels[i]);
		}
		printf("]\n\n");
		printf("***********************************************************************************\n");
		if(!rerun){
			rerun = true;
		}
	}

}

int main(int argc, char** argv) {

	dummy_tester(atoi(argv[1]));
	return 0;
}

