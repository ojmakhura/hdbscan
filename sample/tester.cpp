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
	
	//for(int i = 0; i < 10000; i++){
		printf("***********************************************************************************\n");
		hdbscan scan(minPts, DATATYPE_DOUBLE);
		scan.run(dataset, rows, cols, TRUE);
		
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
	//}

}

int main(int argc, char** argv) {

	dummy_tester(atoi(argv[1]));
	return 0;
}

