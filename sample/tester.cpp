/*
 * tester.cpp
 *
 *  Created on: 19 May 2016
 *      Author: junior
 */

#include "hdbscan.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace clustering;

class HDBSCANStarParameters {
	string inputFile;
	string constraintsFile;
	int minPoints;
	int minClusterSize;
	bool compactHierarchy;
	//DistanceCalculator distanceFunction(EUCLIDEAN);

	string hierarchyFile;
	string clusterTreeFile;
	string partitionFile;
	string outlierScoreFile;
	string visualizationFile;
};

int main(int argc, char** argv) {

	//Parse input parameters from program arguments:
	HDBSCANStarParameters parameters; // = checkInputParameters(args);

	/*System.out.println(
	 "Running HDBSCAN* on " + parameters.inputFile + " with minPts="
	 + parameters.minPoints + ", minClSize="
	 + parameters.minClusterSize + ", constraints="
	 + parameters.constraintsFile + ", compact="
	 + parameters.compactHierarchy + ", dist_function="
	 + parameters.distanceFunction.getName());
	 */
	if(argc < 2){
		printf("Missing argument : example ./hdbscan myfile.csv\n\n");

		return -1;
	}
	//Read in input file:
	HDBSCAN hdbscan(argv[1], EUCLIDEAN, 3, 3);

	//Read in constraints:
	//hdbscan.readInConstraints("example_constraints.csv");

	hdbscan.run(true);

	vector<Cluster*> clusters = *(hdbscan.getClusters());
	printf("Found %d labels.\n[", clusters.size());

	vector<int>* labels = hdbscan.getClusterLabels();

	for(int i = 0; i < labels->size() - 1; i++){
		printf("%d, ", labels[0][i]);
	}

	printf("%d]\n", labels[0][labels->size() - 1]);


	return 0;
}

