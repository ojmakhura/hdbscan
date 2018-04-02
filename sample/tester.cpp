/*
 * ghdbscan.cpp
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
		
		map<int32_t, distance_values> dMap = getMinMaxDistances(scan, clusterTable);
		clustering_stats stats;
		calculateStats(dMap, stats);
		printDistanceMapTable(dMap);
		printStats(stats);
		
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
		break;
	}

}

int main(int argc, char** argv) {

	dummy_tester(atoi(argv[1]));
	return 0;
}

