/*
 * hdbscan.c
 *
 *  Created on: 13 Sep 2017
 *      Author: ojmakh
 */

#include "hdbscan/hdbscan.h"

uint hdbscan_get_dataset_size(int rows, int cols, int rowwise){
    if(rowwise){
        return rows;
    } else{
        return rows * cols;
    }
}

int hdbscan_init(hdbscan* sc, uint minPoints){
	sc = (hdbscan*) malloc(sizeof(hdbscan));

	if(sc == NULL){
		printf("Error: Could not allocate memory for HDBSCAN.\n");
		return HDBSCAN_ERROR;
	}
	sc->minPoints = minPoints;
	int err = distance_init(sc->distanceFunction, _EUCLIDEAN, minPoints);

	if(err == DISTANCE_ERROR){
		printf("Error: Could not create the distance calculator\n");
		return HDBSCAN_ERROR;
	}

	sc->hierarchy = g_hash_table_new(g_int_hash, g_int_equal);
	sc->clusterStabilities = g_hash_table_new(g_int_hash, g_int_equal);

	return HDBSCAN_SUCCESS;
}

void hdbscan_destroy(hdbscan* sc){

	if(sc->clusterLabels != NULL){
		free(sc->clusterLabels);
	}

	if(sc->coreDistances != NULL){
		free(sc->coreDistances);
	}

	if(sc->distanceFunction != NULL){
		distance_destroy(sc->distanceFunction);
	}

	if(sc->mst != NULL){
		graph_destroy(sc->mst);
	}

	if(sc->constraints != NULL){
		constraint_destroy(sc->constraints);
	}

	if(sc->clusters != NULL){
		for(int i = 0; i < g_list_length(sc->clusters); i++){
			cluster* cl = g_list_nth_data(sc->clusters, i);
			cluster_destroy(cl);
		}

		g_list_free(sc->clusters);
	}

	if(sc->clusterStabilities != NULL){

		GList *keys = g_hash_table_get_keys(sc->clusterStabilities);
		GList *values = g_hash_table_get_values(sc->clusterStabilities);

		g_list_free_full(keys, (GDestroyNotify)free);
		g_list_free_full(values, (GDestroyNotify)free);

		g_hash_table_destroy(sc->clusterStabilities);

	}

	if(sc->hierarchy != NULL){

		GList *keys = g_hash_table_get_keys(sc->hierarchy);
		GList *values = g_hash_table_get_values(sc->hierarchy);

		for(int i = 0; i < g_list_length(keys); i++){
			int *key = g_list_nth_data(keys, i);
			GList *val = g_list_nth_data(values, i);
			g_list_free_full(val, (GDestroyNotify)free);
		}
		g_list_free_full(keys, (GDestroyNotify)free);
		g_hash_table_destroy(sc->hierarchy);

	}

	free(sc);
}

int hdbscan_run(hdbscan* sc, double* dataset, uint rows, uint cols, int rowwise){
	sc->numPoints = hdbscan_get_dataset_size(rows, cols, rowwise);
	distance_compute(sc->distanceFunction, dataset, rows, cols, sc->minPoints);
	return hdbscan_run(sc);
}

int hdbscan_run(hdbscan* sc, double* dataset, int size){
	return hdbscan_run(dataset, size, 1, 0);
}

int hdbscan_run(hdbscan* sc){
	if(!hdbscan_construct_mst(sc)){
		printf("Error: Could not construct the minimum spanning tree.\n");
		return HDBSCAN_ERROR;
	}
	graph_quicksort_by_edge_weight(sc->mst);

	double* pointNoiseLevels = (double*)malloc(sc->numPoints * sizeof(double));
	int* pointLastClusters = (int*)malloc(sc->numPoints * sizeof(int));

	hdbscan_compute_hierarchy_and_cluster_tree(sc, 0, pointNoiseLevels, pointLastClusters);
	int infiniteStability = hdbscan_propagate_tree(sc);
	hdbscan_find_prominent_clusters(infiniteStability);

	return HDBSCAN_SUCCESS;
}

void hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int compactHierarchy, double* pointNoiseLevels, int* pointLastClusters, int size){

	int lineCount = 0; // Indicates the number of lines written into
								// hierarchyFile.

	//The current edge being removed from the MST:
	int currentEdgeIndex = sc->mst->esize - 1;
}

int hdbscan_construct_mst(hdbscan* sc){
	double*  coreDistances = sc->distanceFunction->coreDistances;


	int selfEdgeCapacity = 0;
	uint size = sc->numPoints;
	if (sc->selfEdges){
		//printf("Self edges set to true\n");
		selfEdgeCapacity = size;
	}

	//One bit is set (true) for each attached point, or unset (false) for unattached points:
	int attachedPoints[selfEdgeCapacity] = {};
	//The MST is expanded starting with the last point in the data set:
	unsigned int currentPoint = size - 1;
	attachedPoints[size - 1] = 1;


	//Each point has a current neighbor point in the tree, and a current nearest distance:
	int ssize = size - 1 + selfEdgeCapacity;
	int *nearestMRDNeighbors = (int *) malloc(ssize * sizeof(int));
	double *nearestMRDDistances = (double *) malloc(ssize * sizeof(double));//, numeric_limits<double>::max());

	for(int i = 0; i < ssize; i++){
		nearestMRDDistances[i] = DBL_MAX;
	}

	//Create an array for vertices in the tree that each point attached to:
	int *otherVertexIndices = (int *) malloc(ssize * sizeof(int));

	for(uint numAttachedPoints = 1; numAttachedPoints < size; numAttachedPoints++){
		int nearestMRDPoint = -1;
		double nearestMRDDistance = DBL_MAX;


		for (unsigned int neighbor = 0; neighbor < size; neighbor++) {
			if (currentPoint == neighbor) {
				continue;
			}

			if (attachedPoints[neighbor] == 1) {
				continue;
			}

			double mutualReachabiltiyDistance = distance_get(sc->distanceFunction, neighbor, currentPoint);//sc->distanceFunction.getDistance(neighbor, currentPoint);

			if (coreDistances[currentPoint] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[currentPoint];
			}

			if (coreDistances[neighbor] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[neighbor];
			}

			if (mutualReachabiltiyDistance < nearestMRDDistances[neighbor]) {
				nearestMRDDistances[neighbor] = mutualReachabiltiyDistance;
				nearestMRDNeighbors[neighbor] = currentPoint;
			}

			//Check if the unattached point being updated is the closest to the tree:
			if (nearestMRDDistances[neighbor] <= nearestMRDDistance) {
				nearestMRDDistance = nearestMRDDistances[neighbor];
				nearestMRDPoint = neighbor;
			}

		}

		//Attach the closest point found in this iteration to the tree:
		attachedPoints[nearestMRDPoint] = 1;
		otherVertexIndices[numAttachedPoints] = numAttachedPoints;
		//numAttachedPoints++;
		currentPoint = nearestMRDPoint;
	}

	//If necessary, attach self edges:
	if (sc->selfEdges) {
		//size_t n = size * 2 - 1;

		//parallel_for(size_t(0), n, [=](size_t i) {
//#ifdef USE_OPENMP
//#pragma omp parallel for
//#endif
		for (uint i = size - 1; i < size * 2 - 1; i++) {
			int vertex = i - (size - 1);
			nearestMRDNeighbors[i] = vertex;
			otherVertexIndices[i] = vertex;
			nearestMRDDistances[i] = coreDistances[vertex];
			//printf("At %d coreDistances[%d] = %f\n", i, vertex, coreDistances[vertex]);
		}
	}

	int err = graph_init(sc->mst, size, nearestMRDNeighbors, ssize, otherVertexIndices, ssize, nearestMRDDistances, ssize);

	if(err == GRAPH_ERROR){
		printf("Error: Could not initialise mst.\n");
		return HDBSCAN_ERROR;
	}

	return HDBSCAN_SUCCESS;
}

int hdbscan_propagate_tree(hdbscan* sc){

	GHashTable* clustersToExamine = g_hash_table_new(g_int_hash, g_int_equal);
	int addedToExaminationList[g_list_length(sc->clusters)];
	int infiniteStability = FALSE;

	for(int i = 0; i < g_list_length(sc->clusters); i++){
		cluster* cl = g_list_nth_data(sc->clusters, i);

		if(cl != NULL && cl->hasChildren == FALSE){
			g_hash_table_insert(clustersToExamine, &cl->label, cl);
			addedToExaminationList[cl->label] = TRUE;
		} else{

			addedToExaminationList[cl->label] = FALSE;
		}
	}

	while(g_hash_table_size(clustersToExamine) > 0){
		GList *keys = g_hash_table_get_keys(clustersToExamine);
		int *key = g_list_nth_data(keys, g_list_length(keys)-1);
		cluster* currentCluster = g_hash_table_lookup(clustersToExamine, key);
		g_hash_table_remove(clustersToExamine, key);

		cluster_propagate(currentCluster);

		if(currentCluster->stability == DBL_MAX){
			infiniteStability = TRUE;
		}

		if(currentCluster->parent != NULL){
			cluster *parent = currentCluster->parent;

			if(addedToExaminationList[parent->label] == FALSE){
				g_hash_table_insert(clustersToExamine, &parent->label, parent);
				addedToExaminationList[parent->label] = TRUE;
			}
		}

	}

	if(infiniteStability){
		char *message =
					"----------------------------------------------- WARNING -----------------------------------------------\n"
					"(infinite) for some data objects, either due to replicates in the data (not a set) or due to numerical\n"
					"roundings. This does not affect the construction of the density-based clustering hierarchy, but\n"
					"it affects the computation of cluster stability by means of relative excess of mass. For this reason,\n"
					"the post-processing routine to extract a flat partition containing the most stable clusters may\n"
					"produce unexpected results. It may be advisable to increase the value of MinPts and/or M_clSize.\n"
					"-------------------------------------------------------------------------------------------------------";
		printf(message);
	}

	return infiniteStability;
}

void hdbscan_find_prominent_clusters(hdbscan* sc, int infiniteStability){
	cluster* cl = g_list_nth_data(sc->clusters, 1);
	GList *solution = cl->propagatedDescendants;

	sc->clusterLabels = (int *)malloc(sc->numPoints * sizeof(int));

	for(int i = 0; i < sc->numPoints; i++){
		(sc->clusterLabels)[i] = 0;
	}

	GHashTable *significant = g_hash_table_new(g_int_hash, g_int_equal);

}

