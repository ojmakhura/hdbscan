/*
 * hdbscan.c
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

/**
 * @file hdbscan.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Core implementation of the HDBSCAN algorithm
 * 
 * @version 
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "hdbscan/hdbscan.h"
#include <assert.h>
#include <time.h>
#include <math.h>
#include "listlib/set.h"

#ifdef _OPENMP
#include <omp.h>
#endif

/**
 * @brief Calculation of the size of the dataset based on whether it is
 * rowwise or not. If it rowwise, then each row is one value as a vector,
 * otherwise each element dataset[i, j] is an idividual element .
 * 
 * @param rows 
 * @param cols 
 * @param rowwise 
 * @return uint 
 */
uint hdbscan_get_dataset_size(uint rows, uint cols, boolean rowwise){
    if(rowwise == 1){
        return rows;
    } else{
        return rows * cols;
    }
}

/**
 * @brief Removes the set of points from their parent Cluster, and creates a new Cluster, provided the
 * clusterId is not 0 (noise).
 * 
 * @param points The set of points to be in the new Cluster
 * @param clusterLabels An array of cluster labels, which will be modified
 * @param parentCluster The parent Cluster of the new Cluster being created
 * @param clusterLabel The label of the new Cluster
 * @param edgeWeight The edge weight at which to remove the points from their previous Cluster
 * @return cluster*
 */
cluster* hdbscan_create_new_cluster(hdbscan* sc, set_t* points, label_t* clusterLabels, cluster* parentCluster, label_t clusterLabel, distance_t edgeWeight){
	index_t d ;
	#ifdef _OPENMP
	#pragma omp parallel for private(d)
	#endif
	for(index_t i = 0; i < points->size; i++){
		
		d = ((index_t *)points->data)[i];
		clusterLabels[d] = clusterLabel;
	}

	cluster_detach_points(parentCluster, points->size, edgeWeight);
	if (clusterLabel != 0) {
		cluster* new = cluster_init(NULL, clusterLabel, parentCluster, edgeWeight, points->size);
		return new;
	} else{
		cluster_add_points_to_virtual_child_cluster(parentCluster, points);
		return NULL;
	}

}

/**
 * @brief 
 * 
 * @param sc 
 * @param minPoints 
 * @return hdbscan* 
 */
hdbscan* hdbscan_init(hdbscan* sc, index_t minPoints){
	if(sc == NULL)
		sc = (hdbscan*) malloc(sizeof(hdbscan));

	if(sc == NULL){
		printf("Error: Could not allocate memory for HDBSCAN.\n");
	} else{
		sc->minPoints = minPoints;
		sc->selfEdges = TRUE;
		sc->hierarchy = NULL;
		sc->clusterStabilities = NULL;

		sc->constraints = NULL;
		sc->clusterLabels = NULL;
		sc->clusters = NULL;
		sc->coreDistances = NULL;
		sc->outlierScores = NULL;
	}
	return sc;
}

/**
 * @brief 
 * 
 * @param sc 
 */
void hdbscan_minimal_clean(hdbscan* sc){

	if(sc->clusterLabels != NULL){
		free(sc->clusterLabels);
		sc->clusterLabels = NULL;
	}

	if(sc->outlierScores != NULL){
		free(sc->outlierScores);
		sc->outlierScores = NULL;
	}

	if(sc->mst != NULL){
		graph_destroy(sc->mst);
		sc->mst = NULL;
	}

	if(sc->constraints != NULL){

		for(int32_t i = 0; i < sc->constraints->size; i++)
		{
			constraint* c;
			array_list_value_at(sc->constraints, i, &c);
			constraint_destroy(c);
		}

		array_list_delete(sc->constraints);
		sc->constraints = NULL;
	}

	if(sc->hierarchy != NULL){
		hashtable_destroy(sc->hierarchy, NULL, (void (*)(void *))hdbscan_destroy_hierarchical_entry);
		sc->hierarchy = NULL;
	}

	if(sc->clusters != NULL){

		for(size_t i = 0; i < sc->clusters->size; i++)
		{
			cluster* cl;
			array_list_value_at(sc->clusters, i, &cl);
			cluster_destroy(cl);
		}

		array_list_clear(sc->clusters, 0);
	}

	if(sc->clusterStabilities != NULL){

		hashtable_clear(sc->clusterStabilities, NULL, NULL);
	}
}

/**
 * @brief 
 * 
 * @param sc 
 */
void hdbscan_clean(hdbscan* sc){

	distance_clean(&sc->distanceFunction);
	hdbscan_minimal_clean(sc);

	if(sc->clusters != NULL)
	{
		array_list_delete(sc->clusters);
		sc->clusters = NULL;
	}

	if(sc->clusterStabilities != NULL){

		hashtable_destroy(sc->clusterStabilities, NULL, NULL);
		sc->clusterStabilities = NULL;
	}
}

/**
 * @brief 
 * 
 * @param sc 
 */
void hdbscan_destroy(hdbscan* sc){
	hdbscan_clean(sc);

	if(sc != NULL){
		free(sc);
	}
}

/**
 * @brief 
 * 
 * @param sc 
 * @return int 
 */
int hdbscan_do_run(hdbscan* sc){
	index_t csize = sc->numPoints/5;
	if(csize < 4)
	{
		csize = csize * 4;
	}
	
	sc->hierarchy = hashtable_init(csize, H_LONG, H_PTR, long_compare);
	int err = hdbscan_construct_mst(sc);
	
	if(err == HDBSCAN_ERROR){
		printf("Error: Could not construct the minimum spanning tree.\n");
		return HDBSCAN_ERROR;
	}

	//printf("graph_quicksort_by_edge_weight\n");
	graph_quicksort_by_edge_weight(sc->mst);
	

	distance_t pointNoiseLevels[sc->numPoints];
	label_t pointLastClusters[sc->numPoints];

	//printf("hdbscan_compute_hierarchy_and_cluster_tree\n");
	hdbscan_compute_hierarchy_and_cluster_tree(sc, 0, pointNoiseLevels, pointLastClusters);
	//printf("hdbscan_propagate_tree\n");
	int infiniteStability = hdbscan_propagate_tree(sc);

	//printf("hdbscan_find_prominent_clusters\n");
	hdbscan_find_prominent_clusters(sc, infiniteStability);

	return HDBSCAN_SUCCESS;
}

/**
 * @brief 
 * 
 * @param sc 
 * @param minPts 
 * @return int 
 */
int hdbscan_rerun(hdbscan* sc, index_t minPts){
	// clean the hdbscan
	hdbscan_minimal_clean(sc);
	
	sc->selfEdges = TRUE;
	sc->constraints = NULL;
	sc->clusterLabels = NULL;
	sc->coreDistances = NULL;
	sc->outlierScores = NULL;
	sc->minPoints = minPts;
	sc->distanceFunction.numNeighbors = minPts-1;
	distance_get_core_distances(&(sc->distanceFunction));

	return hdbscan_do_run(sc);
}

/**
 * @brief 
 * 
 * @param sc 
 * @param dataset 
 * @param rows 
 * @param cols 
 * @param rowwise 
 * @param datatype 
 * @return int 
 */
int hdbscan_run(hdbscan* sc, void* dataset, index_t rows, index_t cols, boolean rowwise, index_t datatype){

	if(sc == NULL){
		printf("hdbscan_run: sc has not been initialised.\n");
		return HDBSCAN_ERROR;
	}
	//printf("distance_init\n");
	distance_init(&sc->distanceFunction, _EUCLIDEAN, datatype);

	//printf("hdbscan_get_dataset_size\n");
	sc->numPoints = hdbscan_get_dataset_size(rows, cols, rowwise);
	distance_compute(&(sc->distanceFunction), dataset, rows, cols, sc->minPoints-1);

	index_t csize = sc->numPoints/5;
	if(csize < 4)
	{
		csize = csize * 4;
	}
	sc->clusters = ptr_array_list_init(csize, cluster_compare);
	sc->clusterStabilities = hashtable_init(csize, H_INT, H_PTR, int_compare);
	
	return hdbscan_do_run(sc);
}

/**
 * @brief Calculates the number of constraints satisfied by the new clusters and virtual children of the
 * 
 * @param sc 
 * @param newClusterLabels 
 * @param currentClusterLabels 
 */
void hdbscan_calculate_num_constraints_satisfied(hdbscan* sc, set_t* newClusterLabels, label_t* currentClusterLabels){

	if(array_list_size(sc->constraints) == 0)
	{
		return;
	}
}

/**
 * @brief 
 * 
 * @param sc 
 * @param compactHierarchy 
 * @param pointNoiseLevels 
 * @param pointLastClusters 
 * @return int 
 */
int hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int compactHierarchy, distance_t* pointNoiseLevels, label_t* pointLastClusters){

	int64_t lineCount = 0; // Indicates the number of lines written into hierarchyFile.

	//The current edge being removed from the MST:
	// NOTE: This should always be a signed long
	int64_t currentEdgeIndex = sc->mst->edgeWeights->size - 1;

	label_t nextClusterLabel = 2;
	boolean nextLevelSignificant = TRUE;
	//The previous and current cluster numbers of each point in the data set:
	index_t numVertices = sc->mst->numVertices;
	label_t previousClusterLabels[numVertices];
	label_t currentClusterLabels[numVertices];

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(index_t i = 0; i < numVertices; i++){
		previousClusterLabels[i] = 1;
		currentClusterLabels[i] = 1;
	}

	//A list of clusters in the cluster tree, with the 0th cluster (noise) null:
	cluster* c = NULL;
	array_list_append(sc->clusters, &c);

	c = cluster_init(NULL, 1, NULL, NAN, numVertices);
	array_list_append(sc->clusters, &c);

	//Sets for the clusters and vertices that are affected by the edge(s) being removed:
	set_t* affectedClusterLabels = set_init(sizeof(label_t), NULL);

	if(sizeof(label_t) == sizeof(int)) {
		affectedClusterLabels->compare = int_compare;
	} else if(sizeof(label_t) == sizeof(long)) {
		affectedClusterLabels->compare = long_compare;
	} else {
		affectedClusterLabels->compare = short_compare;
	}
	
	set_t* affectedVertices = set_init(sizeof(index_t), NULL);

	if(sizeof(index_t) == sizeof(int)) {
		affectedVertices->compare = int_compare;
	} else if(sizeof(index_t) == sizeof(long)) {
		affectedVertices->compare = long_compare;
	} else {
		affectedVertices->compare = short_compare;
	}

	ArrayList* newClusters = array_list_init(2, sizeof(cluster *), cluster_compare);
	index_t i;
	distance_t currentEdgeWeight, tmp_w;

//#pragma omp parallel
	while (currentEdgeIndex >= 0) {
		
		currentEdgeWeight = ((distance_t *)sc->mst->edgeWeights->data)[currentEdgeIndex];
		if(!array_list_empty(newClusters))
		{
			array_list_clear(newClusters, 0);
		}
		//Remove all edges tied with the current edge weight, and store relevant clusters and vertices:
		tmp_w = ((distance_t *)sc->mst->edgeWeights->data)[currentEdgeIndex];
		index_t firstVertex, secondVertex;
		while(currentEdgeIndex >= 0 && tmp_w == currentEdgeWeight) {
			
			firstVertex = ((index_t *)sc->mst->verticesA->data)[currentEdgeIndex];
			secondVertex = ((index_t *)sc->mst->verticesB->data)[currentEdgeIndex];
			graph_remove_edge(sc->mst, firstVertex, secondVertex);

			if (currentClusterLabels[firstVertex] == 0) {
				currentEdgeIndex--;
				if(currentEdgeIndex >= 0) {
					tmp_w = ((distance_t*)sc->mst->edgeWeights->data)[currentEdgeIndex];
				}

				continue;
			}

			set_insert(affectedVertices, &firstVertex);
			set_insert(affectedVertices, &secondVertex);
			set_insert(affectedClusterLabels, currentClusterLabels + firstVertex);

			currentEdgeIndex--;
			if(currentEdgeIndex >= 0)
			{
				tmp_w = ((distance_t*)sc->mst->edgeWeights->data)[currentEdgeIndex];
			}
		}

		if(affectedClusterLabels->size < 1){
			continue;
		}
		
		/** 
		 * Initialise all the sets to avoid recreating them every loop. Also making sure that we
		 * do not call realloc a lot
		 */
		label_t examinedClusterLabel;
		set_t* examinedVertices = set_init(sizeof(index_t), NULL);
		set_t* firstChildCluster = set_init(sizeof(index_t), NULL);
		set_t* unexploredFirstChildClusterPoints = set_init(sizeof(index_t), NULL);
		set_t* constructingSubCluster = set_init(sizeof(index_t), NULL);
		ArrayList* unexploredSubClusterPoints = array_list_init(examinedVertices->max_size, sizeof(index_t), NULL);

		if(sizeof(index_t) == sizeof(int)) {
			examinedVertices->compare = int_compare;
			firstChildCluster->compare = int_compare;
			unexploredFirstChildClusterPoints->compare = int_compare;
			constructingSubCluster->compare = int_compare;
			unexploredSubClusterPoints->compare = int_compare;
		} else if(sizeof(index_t) == sizeof(long)) {
			examinedVertices->compare = long_compare;
			firstChildCluster->compare = long_compare;
			unexploredFirstChildClusterPoints->compare = long_compare;
			constructingSubCluster->compare = long_compare;
			unexploredSubClusterPoints->compare = long_compare;
		} else {
			examinedVertices->compare = short_compare;
			firstChildCluster->compare = short_compare;
			unexploredFirstChildClusterPoints->compare = short_compare;
			constructingSubCluster->compare = short_compare;
			unexploredSubClusterPoints->compare = short_compare;
		}

		//Check each cluster affected for a possible split:
		while(affectedClusterLabels->size > 0){

			set_remove_at(affectedClusterLabels, affectedClusterLabels->size-1, &examinedClusterLabel);

			//Get all affected vertices that are members of the cluster currently being examined:
			index_t it = 0;
			index_t n;

			while(it < affectedVertices->size){
				
				n = ((index_t *)affectedVertices->data)[it];
				if (currentClusterLabels[n] == examinedClusterLabel) {
					set_insert(examinedVertices, &n);
					set_remove(affectedVertices, &n);
				} else{
					it++;
				}
			}

			index_t numChildClusters = 0;

			/* Check if the cluster has split or shrunk by exploring the graph from each affected
			 * vertex.  If there are two or more valid child clusters (each has >= minClusterSize
			 * points), the cluster has split.
			 * Note that firstChildCluster will only be fully explored if there is a cluster
			 * split, otherwise, only spurious components are fully explored, in order to label
			 * them noise.
			 */
			while (examinedVertices->size > 0) {

				boolean anyEdges = FALSE;
				boolean incrementedChildCount = FALSE;
				index_t rootVertex;

				set_remove_at(examinedVertices, examinedVertices->size-1, &rootVertex);
				set_insert(constructingSubCluster, &rootVertex);
				array_list_append(unexploredSubClusterPoints, &rootVertex);

				//Explore this potential child cluster as long as there are unexplored points:
				while (unexploredSubClusterPoints->size > 0) {
					index_t vertexToExplore;
					array_list_pop(unexploredSubClusterPoints, &vertexToExplore);

					ArrayList* v = sc->mst->edges[vertexToExplore];
					index_t neighbor;
					
					for(i = 0; i < v->size; i++){
						
						neighbor = ((index_t*)v->data)[i];
						anyEdges = TRUE;
						boolean p = set_insert(constructingSubCluster, &neighbor);

						if(p){
							array_list_append(unexploredSubClusterPoints, &neighbor);
							set_remove(examinedVertices, &neighbor);
						}
					}

					//Check if this potential child cluster is a valid cluster:
					if(incrementedChildCount == FALSE && constructingSubCluster->size >= sc->minPoints && anyEdges == TRUE){
						incrementedChildCount = TRUE;
						numChildClusters++;

						//If this is the first valid child cluster, stop exploring it:
						if (firstChildCluster->size == 0) {
							index_t d;
							for(i = 0; i < constructingSubCluster->size; i++){
								d = ((index_t*)constructingSubCluster->data)[i];
								set_insert(firstChildCluster, &d);
							}

							for(i = 0; i < unexploredSubClusterPoints->size; i++){
								d = ((index_t*)unexploredSubClusterPoints->data)[i];
								set_insert(unexploredFirstChildClusterPoints, &d);
							}
							break;
						}
					}
				}
				cluster* examinedCluster = NULL;
				//If there could be a split, and this child cluster is valid:
				if(numChildClusters >= 2 && constructingSubCluster->size >= sc->minPoints && anyEdges == TRUE){

					//Check this child cluster is not equal to the unexplored first child cluster:
					index_t firstChildClusterMember = ((index_t*)firstChildCluster->data)[firstChildCluster->size-1];

					if(set_find(constructingSubCluster, &firstChildClusterMember) > -1){
						numChildClusters--;
					}

					//Otherwise, create a new cluster:
					else {
						
						examinedCluster = ((cluster**)sc->clusters->data)[examinedClusterLabel];
						cluster* newCluster = hdbscan_create_new_cluster(sc, constructingSubCluster, currentClusterLabels, 
																			examinedCluster, nextClusterLabel, currentEdgeWeight);
						array_list_append(newClusters, &newCluster);
						nextClusterLabel++;
						array_list_append(sc->clusters, &newCluster);
					}
				}

				//If this child cluster is not valid cluster, assign it to noise:
				else if(constructingSubCluster->size < sc->minPoints || anyEdges == FALSE){

					examinedCluster = ((cluster**)sc->clusters->data)[examinedClusterLabel];
					cluster* newCluster = hdbscan_create_new_cluster(sc, constructingSubCluster, currentClusterLabels, examinedCluster, 0, currentEdgeWeight);
					index_t point;
					#ifdef _OPENMP
					#pragma omp parallel for private(point)
					#endif
					for (i = 0; i < constructingSubCluster->size; i++) {
						
						point = ((index_t*)constructingSubCluster->data)[i];
						pointNoiseLevels[point] = currentEdgeWeight;
						pointLastClusters[point] = examinedClusterLabel;
					}

					cluster_destroy(newCluster);

				}
				/*************************************
				 * Clean up constructing subcluster
				 *************************************/
				constructingSubCluster->size = 0;
				array_list_clear(unexploredSubClusterPoints, 0);
			}
			
			//Finish exploring and cluster the first child cluster if there was a split and it was not already clustered:
			index_t dd = ((index_t*)firstChildCluster->data)[0];
			index_t neighbor;
			index_t vertexToExplore;	

			if (numChildClusters >= 2 && currentClusterLabels[dd] == examinedClusterLabel) {
				while(unexploredFirstChildClusterPoints->size > 0){
					
					set_remove_at(unexploredFirstChildClusterPoints, unexploredFirstChildClusterPoints->size-1, &vertexToExplore);
					ArrayList* v = (sc->mst->edges)[vertexToExplore];

					for (i = 0; i < v->size; i++) {
						neighbor = ((index_t*)v->data)[i];
						if (set_insert(firstChildCluster, &neighbor)) {
							set_insert(unexploredFirstChildClusterPoints, &neighbor);
						}
					}
				}

				cluster* examinedCluster = NULL;
				examinedCluster = ((cluster**)sc->clusters->data)[examinedClusterLabel];
				cluster* newCluster = hdbscan_create_new_cluster(sc, firstChildCluster, currentClusterLabels, examinedCluster, nextClusterLabel, currentEdgeWeight);
				array_list_append(newClusters, &newCluster);
				nextClusterLabel++;
				array_list_append(sc->clusters, &newCluster);
			}
			
			firstChildCluster->size = 0;
			unexploredFirstChildClusterPoints->size = 0;
			examinedVertices->size = 0;
		}
		set_delete(constructingSubCluster);
		array_list_delete(unexploredSubClusterPoints);
		set_delete(examinedVertices);
		set_delete(firstChildCluster);
		set_delete(unexploredFirstChildClusterPoints);

		if (compactHierarchy == FALSE || nextLevelSignificant == TRUE || array_list_size(newClusters) > 0) {
			lineCount++;
			hierarchy_entry* entry = hdbscan_create_hierarchy_entry();
			entry->edgeWeight = currentEdgeWeight;
			entry->labels = (label_t*)malloc(numVertices * sizeof(label_t));

			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			for(i = 0; i < numVertices; i++)
			{
				entry->labels[i] = previousClusterLabels[i];
			}

			hashtable_insert(sc->hierarchy, &lineCount, &entry);
		}

		// Assign offsets and calculate the number of constraints satisfied:
		//set_t* newClusterLabels = set_init(sizeof(long), long_compare);

		cluster* newCluster = NULL;
		
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for(i = 0; i < array_list_size(newClusters); i++)
		{
			//array_list_value_at(newClusters, i, &newCluster);
			newCluster = ((cluster**)newClusters->data)[i];
			newCluster->offset = lineCount;
			//set_insert(newClusterLabels, &lineCount);
		}
		
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for (i = 0; i < numVertices; i++) {
			previousClusterLabels[i] = currentClusterLabels[i];
		}

		if (array_list_size(newClusters) == 0){
			nextLevelSignificant = FALSE;
		} else{
			nextLevelSignificant = TRUE;
		}

		array_list_clear(newClusters, 0);
		//set_delete(newClusterLabels);
	}
	array_list_delete(newClusters);

	hierarchy_entry* entry = hdbscan_create_hierarchy_entry();
	entry->edgeWeight = 0.0;
	entry->labels = (label_t*) malloc(numVertices * sizeof(label_t));
	
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(i = 0; i < numVertices; i++)
	{
		entry->labels[i] = 0;
	}	

	// Write out the final level of the hierarchy (all points noise):
	int64_t l = 0;
	hashtable_insert(sc->hierarchy, &l, &entry);
	lineCount++;

	set_delete(affectedClusterLabels);
	set_delete(affectedVertices);

	return HDBSCAN_SUCCESS;
}

/**
 * @brief 
 * 
 * @return hierarchy_entry* 
 */
hierarchy_entry* hdbscan_create_hierarchy_entry(){
	hierarchy_entry* entry = (hierarchy_entry *) malloc(sizeof(hierarchy_entry));
	entry->labels = NULL;
	entry->edgeWeight = 0.0;
	return entry;
}

/**
 * @brief 
 * 
 * @param sc 
 */
void print_distances(hdbscan* sc){
	for (index_t i = 0; i < sc->numPoints; i++) {
		printf("[");
		for (index_t j = 0; j < sc->numPoints; j++) {
			printf("%f ", distance_get(&sc->distanceFunction, i, j));
		}

		printf("]\n");
	}
	printf("\n");
}

/**
 * @brief 
 * 
 * @param nearestMRDNeighbors 
 * @param otherVertexIndices 
 * @param nearestMRDDistances 
 */
void print_graph_components(ArrayList *nearestMRDNeighbors, ArrayList *otherVertexIndices, ArrayList *nearestMRDDistances){
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>nearestMRDNeighbors>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	for(size_t i = 0; i < nearestMRDNeighbors->size; i++){

		index_t a = ((index_t *)nearestMRDNeighbors->data)[i];
		index_t b = ((index_t *)otherVertexIndices->data)[i];
		distance_t d = ((distance_t *)nearestMRDDistances->data)[i];

		printf("%ld --- (%d, %d) : %f\n", i, a, b, d);
	}

	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n\n");
}

/**
 * @brief 
 * 
 * @param sc 
 * @return int 
 */
int hdbscan_construct_mst(hdbscan* sc){
	distance_t*  coreDistances = sc->distanceFunction.coreDistances;

	int selfEdgeCapacity = 0;
	index_t size = sc->numPoints;
	if (sc->selfEdges == TRUE){
		selfEdgeCapacity = size;
	}

	//One bit is set (true) for each attached point, or unset (false) for unattached points:
	boolean attachedPoints[size];
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(index_t i = 0; i < size-1; i++){
		attachedPoints[i] = FALSE;
	}

	//The MST is expanded starting with the last point in the data set:
	index_t currentPoint = size - 1;
	attachedPoints[size - 1] = TRUE;

	//Each point has a current neighbor point in the tree, and a current nearest distance:
	index_t ssize = size - 1 + selfEdgeCapacity;
	ArrayList* nearestMRDNeighbors = array_list_init(ssize, sizeof(index_t), NULL);
	
	//Create an array for vertices in the tree that each point attached to:
	ArrayList* otherVertexIndices = array_list_init(ssize, sizeof(index_t), NULL);

	if(nearestMRDNeighbors == NULL){
		printf("ERROR: hdbscan_construct_mst - Could not construct nearestMRDNeighbors");
		return HDBSCAN_ERROR;
	}

	if(otherVertexIndices == NULL){
		printf("ERROR: hdbscan_construct_mst - Could not construct otherVertexIndices");
		return HDBSCAN_ERROR;
	}

	if(sizeof(index_t) == sizeof(int)) {
		nearestMRDNeighbors->compare = int_compare;
		otherVertexIndices->compare = int_compare;
	} else if(sizeof(index_t) == sizeof(long)) {
		nearestMRDNeighbors->compare = long_compare;
		otherVertexIndices->compare = long_compare;
	} else {
		nearestMRDNeighbors->compare = short_compare;
		otherVertexIndices->compare = short_compare;
	}

	ArrayList* nearestMRDDistances = array_list_init(ssize, sizeof(distance_t), NULL);
	if(sizeof(distance_t) == sizeof(double))
	{
		nearestMRDDistances->compare = double_compare;
	} else {
		nearestMRDDistances->compare = float_compare;
	}
	
	// User these throughout the loop
	distance_t* distances = nearestMRDDistances->data;
	index_t* neighbours = nearestMRDNeighbors->data;
	index_t* others = otherVertexIndices->data;

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(index_t i = 0; i < ssize; i++) {
		
		distances[i] = D_MAX;
		neighbours[i] = 0;
		others[i] = 0;
	}

	nearestMRDDistances->size = ssize;
	nearestMRDNeighbors->size = ssize;
	otherVertexIndices->size = ssize;

	if(nearestMRDDistances == NULL){
		printf("ERROR: hdbscan_construct_mst - Could not construct nearestMRDDistances");
		return HDBSCAN_ERROR;
	}

	//Continue attaching points to the MST until all points are attached:
	for (index_t numAttachedPoints = 1; numAttachedPoints < size; numAttachedPoints++) {
		int nearestMRDPoint = -1;
		distance_t nearestMRDDistance = D_MAX;

		//Iterate through all unattached points, updating distances using the current point:
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for (index_t neighbor = 0; neighbor < size; neighbor++) {

			if (currentPoint == neighbor) {
				continue;
			}

			if (attachedPoints[neighbor] == TRUE) {
				continue;
			}
			
			distance_t mutualReachabiltiyDistance = distance_get(&sc->distanceFunction, neighbor, currentPoint);

			if (coreDistances[currentPoint] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[currentPoint];
			}

			if (coreDistances[neighbor] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[neighbor];
			}

			distance_t d = ((distance_t*)nearestMRDDistances->data)[neighbor];
			if (mutualReachabiltiyDistance < d) {
				distances[neighbor] = mutualReachabiltiyDistance;
				neighbours[neighbor] = currentPoint;
			}

			//Check if the unattached point being updated is the closest to the tree:
			d = distances[neighbor];
			if (d <= nearestMRDDistance) {
				nearestMRDDistance = d;
				nearestMRDPoint = neighbor;
			}
		}

		//Attach the closest point found in this iteration to the tree:
		attachedPoints[nearestMRDPoint] = TRUE;
		others[numAttachedPoints] = numAttachedPoints;
		currentPoint = nearestMRDPoint;
	}

	//If necessary, attach self edges:
	if (sc->selfEdges == TRUE) {
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for (index_t i = size - 1; i < size * 2 - 1; i++) {
			index_t vertex = i - (size - 1);
			neighbours[i] = vertex;
			others[i] = vertex;
			distances[i] = coreDistances[vertex];
		}
	}

	sc->mst = graph_init(NULL, size, nearestMRDNeighbors, otherVertexIndices, nearestMRDDistances);
	if(sc->mst == NULL){
		printf("Error: Could not initialise mst.\n");
		return HDBSCAN_ERROR;
	}

	return HDBSCAN_SUCCESS;
}

/**
 * @brief 
 * 
 * @param sc 
 * @return boolean 
 */
boolean hdbscan_propagate_tree(hdbscan* sc){

	set_t* clustersToExamine = set_init(sizeof(label_t), NULL);
	
	if(sizeof(label_t) == sizeof(int)) {
		clustersToExamine->compare = int_compare;
	} else if(sizeof(label_t) == sizeof(long)) {
		clustersToExamine->compare = long_compare;
	} else {
		clustersToExamine->compare = short_compare;
	}

	boolean addedToExaminationList[sc->clusters->size];
	boolean infiniteStability = FALSE;

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(size_t i = 0; i < sc->clusters->size; i++){
		addedToExaminationList[i] = FALSE;
	}

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(size_t i = 0; i < sc->clusters->size; i++){

		cluster* cl = ((cluster**)sc->clusters->data)[i];
		if(cl != NULL && cl->hasChildren == FALSE){
			#ifdef _OPENMP
			#pragma omp critical
			#endif
			{
			label_t tm = cl->label;
			set_insert(clustersToExamine, &tm);
			addedToExaminationList[cl->label] = TRUE;
			}
		}
	}

	label_t x;
	while(!set_empty(clustersToExamine)){
		
		set_remove_at(clustersToExamine, clustersToExamine->size-1, &x);
		cluster* currentCluster = ((cluster **)sc->clusters->data)[x];
		cluster_propagate(currentCluster);

		if(currentCluster->stability == D_MAX){
			infiniteStability = TRUE;
		}
		if(currentCluster->parent != NULL){
			cluster *parent = currentCluster->parent;

			if(addedToExaminationList[parent->label] == FALSE){
				label_t tm = parent->label;
				set_insert(clustersToExamine, &tm);
				addedToExaminationList[parent->label] = TRUE;
			}
		}
	}

	if(infiniteStability){
		const char *message =
					"----------------------------------------------- WARNING -----------------------------------------------\n"
					"(infinite) for some data objects, either due to replicates in the data (not a set) or due to numerical\n"
					"roundings. This does not affect the construction of the density-based clustering hierarchy, but\n"
					"it affects the computation of cluster stability by means of relative excess of mass. For this reason,\n"
					"the post-processing routine to extract a flat partition containing the most stable clusters may\n"
					"produce unexpected results. It may be advisable to increase the value of MinPts and/or M_clSize.\n"
					"-------------------------------------------------------------------------------------------------------";
		printf(message);
	}
	
	set_delete(clustersToExamine);

	return infiniteStability;
}

/**
 * @brief 
 * 
 * @param sc 
 * @param infiniteStability 
 */
void hdbscan_find_prominent_clusters(hdbscan* sc, int infiniteStability){
	
	cluster* cl = ((cluster **) sc->clusters->data)[1];
	ArrayList *solution = cl->propagatedDescendants;
	
	hashtable *significant = hashtable_init(array_list_size(solution), H_LONG, H_PTR, long_compare);
	int32_t ret;
	cluster* c = NULL;

	#ifdef _OPENMP
	#pragma omp parallel for private(ret, c)
	#endif
	for(int32_t i = 0; i < solution->size; i++){
		
		#ifdef _OPENMP
		#pragma omp critical
		#endif
		{
		ret = array_list_value_at(solution, i, &c);

		if(ret != 0){
			ArrayList* clusterList = NULL;
			ret = hashtable_lookup(significant, &c->offset, &clusterList);

			if(clusterList == NULL){
				clusterList = array_list_init(16, sizeof(label_t), NULL);
				if(sizeof(label_t) == sizeof(int)) {
					clusterList->compare = int_compare;
				} else if(sizeof(label_t) == sizeof(long)) {
					clusterList->compare = long_compare;
				} else {
					clusterList->compare = short_compare;
				}
				long tmp = c->offset;
				hashtable_insert(significant, &tmp, &clusterList);
			}
			
			array_list_append(clusterList, &(c->label));
		}
		}
	}
	
	sc->clusterLabels = (label_t *)calloc(sc->numPoints, sizeof(label_t));

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	for(size_t i = 0; i < set_size(significant->keys); i++)
	{
		int64_t key = ((int64_t *)significant->keys->data)[i];

		ArrayList* clusterList = NULL;
		hashtable_lookup(significant, &key, &clusterList);
		hierarchy_entry* hpSecond;
		int64_t l = key + 1;
		
		hashtable_lookup(sc->hierarchy, &l, &hpSecond);
		for(index_t j = 0; j < sc->numPoints; j++){
			label_t label = (hpSecond->labels)[j];
			int it = array_list_find(clusterList, &label, 0);
			if(it != -1){
				sc->clusterLabels[j] = label;
			}
		}
	}

	hashtable_destroy(significant, NULL, (void (*)(void *))array_list_delete);
}

/**
 * @brief 
 * 
 * @param sc 
 * @param pointNoiseLevels 
 * @param pointLastClusters 
 * @param infiniteStability 
 * @return int 
 */
int hdbscsan_calculate_outlier_scores(hdbscan* sc, distance_t* pointNoiseLevels, label_t* pointLastClusters, boolean infiniteStability){

	distance_t* coreDistances = sc->distanceFunction.coreDistances;
	index_t numPoints = sc->numPoints;
	sc->outlierScores = (outlier_score*)malloc(numPoints*sizeof(outlier_score));

	if(!sc->outlierScores){
		printf("ERROR: Calculate Outlier Score - Could not allocate memory for outlier scores.\n");
		return HDBSCAN_ERROR;
	}

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(uint i = 0; i < sc->numPoints; i++){
		label_t tmp = pointLastClusters[i];
		cluster* c = ((cluster **)sc->clusters->data)[tmp];
		distance_t epsilon_max = c->propagatedLowestChildDeathLevel;
		distance_t epsilon = pointNoiseLevels[i];

		distance_t score = 0;
		if(epsilon != 0){
			score = 1 - (epsilon_max / epsilon);
		}

		sc->outlierScores[i].id = i;
		sc->outlierScores[i].score = score;
		sc->outlierScores[i].coreDistance = coreDistances[i];
	}

	//Sort the outlier scores:
	qsort(sc->outlierScores, numPoints, sizeof(outlier_score), outlier_score_compare);

	return 1;
}

/**
 * @brief 
 * 
 * @param labels 
 * @param begin 
 * @param end 
 * @return hashtable* 
 */
hashtable* hdbscan_create_cluster_map(label_t* labels, int32_t begin, int32_t end){
	index_t bsize = (end - begin)/4;
	hashtable* clusterTable;
	
	if(sizeof(label_t) == sizeof(int)) {
		clusterTable = hashtable_init(bsize, H_INT, H_PTR, int_compare);
	} else if(sizeof(label_t) == sizeof(long)) {
		clusterTable = hashtable_init(bsize, H_LONG, H_PTR, long_compare);
	} else {
		clusterTable = hashtable_init(bsize, H_SHORT, H_PTR, short_compare);		
	}

	index_t size = end - begin;
	
	for(index_t i = begin; i < end; i++){
		label_t *lb = labels + i;
		ArrayList* clusterList = NULL;
		hashtable_lookup(clusterTable, lb, &clusterList);

		if(clusterList == NULL){
			clusterList = array_list_init(size/3, sizeof(index_t), NULL);
			if(sizeof(index_t) == sizeof(int)) {
				clusterList->compare = int_compare;
			} else if(sizeof(index_t) == sizeof(long)) {
				clusterList->compare = long_compare;
			} else {
				clusterList->compare = short_compare;		
			}
			hashtable_insert(clusterTable, lb, &clusterList);
		}
		array_list_append(clusterList, &i);
	}

	return clusterTable;
}

/**
 * @brief 
 * 
 * @param sc 
 * @param clusterTable 
 * @return hashtable* 
 */
hashtable* hdbscan_get_min_max_distances(hdbscan* sc, hashtable* clusterTable){

	hashtable* distanceMap;

	if(sizeof(label_t) == sizeof(int)) {
		distanceMap = hashtable_init_size(clusterTable->size, H_INT, H_PTR, int_compare);
	} else if(sizeof(label_t) == sizeof(long)) {
		distanceMap = hashtable_init_size(clusterTable->size, H_LONG, H_PTR, long_compare);
	} else {
		distanceMap = hashtable_init_size(clusterTable->size, H_SHORT, H_PTR, short_compare);		
	}

	distance_t* core = sc->distanceFunction.coreDistances;
	distance_t zero = 0.0000000000000000000;
	label_t key;

	for(size_t i = 0; i < set_size(clusterTable->keys); i++)
    {
		key = ((label_t *)clusterTable->keys->data)[i];
		ArrayList* clusterList = NULL;		
        hashtable_lookup(clusterTable, &key, &clusterList);
		index_t* idxList = clusterList->data;

		distance_values* dl = NULL;
		hashtable_lookup(distanceMap, &key, &dl);
		
		for(size_t j = 0; j < clusterList->size; j++){
			index_t index = idxList[j];
			if(dl == NULL)
			{
				dl = (distance_values *)malloc(sizeof(distance_values));
				int32_t index = idxList[j];
				dl->min_cr = core[index];
				dl->max_cr = core[index];
				dl->cr_confidence = 0.0;

				dl->min_dr = D_MAX;
				dl->max_dr = D_MIN;
				dl->dr_confidence = 0.0;
				hashtable_insert(distanceMap, &key, &dl);
			} else {

				if(dl->min_cr > core[index] && (core[index] < zero || core[index] > zero)){
					dl->min_cr = core[index];
				}

				//max core distance
				if(dl->max_cr < core[index]){
					dl->max_cr = core[index];
				}
			}
			
			// Calculating min and max distances
			for(size_t k = j+1; k < clusterList->size; k++){
				distance_t d = distance_get(&(sc->distanceFunction), index, idxList[k]);

				if(dl->min_dr > d && (d < zero || d > zero)){
					dl->min_dr = d;
				}

				// max distance
				if(dl->max_dr < d){
					dl->max_dr = d;
				}
			}
		}
    }

	return distanceMap;
}

/**
 * @brief 
 * 
 * @param table 
 */
void hdbscan_destroy_distance_map(hashtable* table){

    hashtable_destroy(table, NULL, free);
	table = NULL;
}

/**
 * @brief Calculate skewness and kurtosis using the equations from 
 * https://www.gnu.org/software/gsl/doc/html/statistics.html
 * 
 * @param stats 
 * @param sum_sc 
 * @param sum_sd 
 * @param sum_dc 
 * @param sum_dd 
 */
void hdbscan_skew_kurt_1(clustering_stats* stats, distance_t sum_sc, distance_t sum_sd, distance_t sum_dc, distance_t sum_dd)
{
	#ifdef _OPENMP
	#pragma omp parallel
	{
	#endif
	int32_t N = stats->count;
	stats->coreDistanceValues.skewness = sum_sc / (N * pow(stats->coreDistanceValues.standardDev, 3));
	stats->intraDistanceValues.skewness = sum_sd / (N * pow(stats->intraDistanceValues.standardDev, 3));

	stats->coreDistanceValues.kurtosis = (sum_dc / (N * pow(stats->coreDistanceValues.standardDev, 4))) - 3;
	stats->intraDistanceValues.kurtosis = (sum_dd / (N *pow(stats->intraDistanceValues.standardDev, 4))) - 3;
	#ifdef _OPENMP
	}
	#endif
}

/**
 * @brief Calculate skewness and kurtosis in the same way as in MS excel
 * https://support.office.com/en-us/article/skew-function-bdf49d86-b1ef-4804-a046-28eaea69c9fa
 * https://support.office.com/en-ie/article/kurt-function-bc3a265c-5da4-4dcb-b7fd-c237789095ab
 * 
 * @param stats 
 * @param sum_sc 
 * @param sum_sd 
 * @param sum_dc 
 * @param sum_dd 
 */
void hdbscan_skew_kurt_2(clustering_stats* stats, distance_t sum_sc, distance_t sum_sd, distance_t sum_dc, distance_t sum_dd)
{

	#ifdef _OPENMP
	#pragma omp parallel
	{
	#endif
	int32_t N = stats->count;
	
	// Calculate the skewness
	if(stats->count >= 2){
		distance_t tmp1 = ((distance_t)N) / ((N - 1) * (N - 2));
		stats->coreDistanceValues.skewness = tmp1 * (sum_sc / pow(stats->coreDistanceValues.standardDev, 3));
		stats->intraDistanceValues.skewness = tmp1 * (sum_sd / pow(stats->intraDistanceValues.standardDev, 3));
	} else {
		stats->coreDistanceValues.skewness = 0.0/0.0;
		stats->intraDistanceValues.skewness = 0.0/0.0;
	}

	// Calculate the kurtosis
	if(stats->count >= 3){
		distance_t tmp2 = (((distance_t)N) * (N + 1)) / ((N - 1) * (N - 2) * (N - 3));
		stats->coreDistanceValues.kurtosis = tmp2 * (sum_dc / pow(stats->coreDistanceValues.standardDev, 4));
		stats->intraDistanceValues.kurtosis = tmp2 * (sum_dd / pow(stats->intraDistanceValues.standardDev, 4));
	}

	if(stats->count >= 3){
		distance_t tmp3 = (3 * (((distance_t)N) - 1) * (N - 1)) / ((N - 2) * (N - 3));
		stats->coreDistanceValues.kurtosis -= tmp3;
		stats->intraDistanceValues.kurtosis -= tmp3;
	} else {

		stats->coreDistanceValues.kurtosis = 0.0/0.0;
		stats->intraDistanceValues.kurtosis = 0.0/0.0;
	}
	#ifdef _OPENMP
	}
	#endif
}

/**
 * Calculate the skewness and kurtosis using the the gsl library.
 * 
 * https://www.gnu.org/software/gsl/doc/html/statistics.html
 */ 
void hdbscan_skew_kurt_gsl(clustering_stats* stats, distance_t* cr, distance_t* dr)
{
	/*
	stats->coreDistanceValues.standardDev = gsl_stats_sd(cr, 1, stats->count);
	stats->coreDistanceValues.variance = gsl_stats_variance(cr, 1, stats->count);
	stats->coreDistanceValues.mean = gsl_stats_mean(cr, 1, stats->count);
	stats->coreDistanceValues.kurtosis = gsl_stats_kurtosis_m_sd(cr, 1, stats->count, stats->coreDistanceValues.mean, stats->coreDistanceValues.standardDev);
	stats->coreDistanceValues.skewness = gsl_stats_skew_m_sd(cr, 1, stats->count, stats->coreDistanceValues.mean, stats->coreDistanceValues.standardDev);
	stats->coreDistanceValues.max = gsl_stats_max(cr, 1, stats->count);

	// calculating intra distance statistics
	stats->intraDistanceValues.standardDev = gsl_stats_sd(dr, 1, stats->count);
	stats->intraDistanceValues.variance = gsl_stats_variance(dr, 1, stats->count);
	stats->intraDistanceValues.mean = gsl_stats_mean(dr, 1, stats->count);
	stats->intraDistanceValues.kurtosis = gsl_stats_kurtosis_m_sd(dr, 1, stats->count, stats->coreDistanceValues.mean, stats->coreDistanceValues.standardDev);
	stats->intraDistanceValues.skewness = gsl_stats_skew_m_sd(dr, 1, stats->count, stats->coreDistanceValues.mean, stats->coreDistanceValues.standardDev);
	stats->intraDistanceValues.max = gsl_stats_max(dr, 1, stats->count);
	*/
}

/**
 * @brief 
 * 
 */
void hdbscan_calculate_stats_helper(distance_t* cr, distance_t* dr, clustering_stats* stats){
	
	stats->coreDistanceValues.mean = cr[0];
	stats->coreDistanceValues.max = cr[0];

	stats->intraDistanceValues.mean = dr[0];
	stats->intraDistanceValues.max = dr[0];

	for(int32_t i = 1; i < stats->count; i++)
	{
		// Core distance statistics
		if(cr[i] > stats->coreDistanceValues.max)
		{
			stats->coreDistanceValues.max = cr[i];
		}

		stats->coreDistanceValues.mean += cr[i];

		// Intra cluster distances
		if(dr[i] > stats->intraDistanceValues.max)
		{
			stats->intraDistanceValues.max = dr[i];
		}

		stats->intraDistanceValues.mean += dr[i];
	}

	// Get the average
	stats->coreDistanceValues.mean = stats->coreDistanceValues.mean/stats->count;
	stats->intraDistanceValues.mean = stats->intraDistanceValues.mean/stats->count;

	/// Calculate the variance
	distance_t sum_sc = 0;
	distance_t sum_sd = 0;
	distance_t sum_dc = 0;
	distance_t sum_dd = 0;
	
	for(int32_t i = 0; i < stats->count; i++)
	{
		distance_t tmp_c = cr[i] - stats->coreDistanceValues.mean;
		stats->coreDistanceValues.variance += tmp_c * tmp_c;
		sum_sc += pow(tmp_c, 3);
		sum_dc += pow(tmp_c, 4);

		distance_t tmp_d = dr[i] - stats->intraDistanceValues.mean;
		stats->intraDistanceValues.variance += tmp_d * tmp_d;
		sum_sd += pow(tmp_d, 3);
		sum_dd += pow(tmp_d, 4);
	}

	stats->coreDistanceValues.variance = stats->coreDistanceValues.variance / (stats->count - 1);
	stats->intraDistanceValues.variance = stats->intraDistanceValues.variance / (stats->count - 1);

	stats->coreDistanceValues.standardDev = sqrt(stats->coreDistanceValues.variance);
	stats->intraDistanceValues.standardDev = sqrt(stats->intraDistanceValues.variance);

	//hdbscan_skew_kurt_1(stats, sum_sc, sum_sd, sum_dc, sum_dd);
	hdbscan_skew_kurt_2(stats, sum_sc, sum_sd, sum_dc, sum_dd);
	//hdbscan_skew_kurt_gsl(stats, cr, dr);
	
}

/**
 * @brief 
 * 
 * @param distanceMap 
 * @param stats 
 */
void hdbscan_calculate_stats(hashtable* distanceMap, clustering_stats* stats){

	distance_t cr[hashtable_size(distanceMap)];
	distance_t dr[hashtable_size(distanceMap)];
	label_t key;
	distance_values* dl = NULL;
	
	#ifdef _OPENMP
	#pragma omp parallel for private (key, dl)
	#endif
	for(size_t i = 0; i < hashtable_size(distanceMap); i++)
	{
		key = ((label_t *)distanceMap->keys->data)[i];
		hashtable_lookup(distanceMap, &key, &dl);
		cr[i] = dl->max_cr/dl->min_cr;
		dr[i] = dl->max_dr/dl->min_dr;
	}

	stats->count = hashtable_size(distanceMap);
	hdbscan_calculate_stats_helper(cr, dr, stats);

	//#ifdef _OPENMP
	//#pragma omp parallel for
	//#endif
	for(size_t i = 0; i < hashtable_size(distanceMap); i++)
	{
		key = ((label_t *)distanceMap->keys->data)[i];
		hashtable_lookup(distanceMap, &key, &dl);
		distance_t rc = cr[i];
		distance_t rd = dr[i];

		dl->cr_confidence = ((stats->coreDistanceValues.max - rc) / stats->coreDistanceValues.max) * 100;
		dl->dr_confidence = ((stats->intraDistanceValues.max - rd) / stats->intraDistanceValues.max) * 100;
	}
}

/**
 * @brief 
 * 
 * @param stats 
 * @return int32_t 
 */
int32_t hdbscan_analyse_stats(clustering_stats* stats){

	int32_t validity = 0;
	distance_t skew_cr = stats->coreDistanceValues.skewness;
	distance_t skew_dr = stats->intraDistanceValues.skewness;
	distance_t kurtosis_cr = stats->coreDistanceValues.kurtosis;
	distance_t kurtosis_dr = stats->intraDistanceValues.kurtosis;

	if((skew_dr > 0.0 ) && (kurtosis_dr > 0.0 )){
		validity += 2;
	} else if(skew_dr < 0.0 && kurtosis_dr > 0.0){
		validity += 1;
	} else if(skew_dr > 0.0 && kurtosis_dr < 0.0){
		validity += 0;
	} else{
		validity += -1;
	}

	if((skew_cr > 0.0 ) && (kurtosis_cr > 0.0 )){
		validity += 2;
	} else if(skew_cr < 0.0 && kurtosis_cr > 0.0){
		validity += 1;
	} else if(skew_cr > 0.0 && kurtosis_cr < 0.0){
		validity += 0;
	} else{
		validity += -1;
	}

	return validity;
}

/**
 * @brief 
 * 
 * @param c_data 
 * @param d_data 
 * @param lower 
 * @param upper 
 * @param pivot 
 */
void partition(label_t *c_data, distance_t *d_data, int lower, int upper, int* pivot){
	distance_t x = d_data[lower];
	label_t x2 = c_data[lower];

	int up = lower+1; /* index will go up */
	int down = upper; /* index will go down */

	while(up < down){
		while((up < down) && (d_data[up] <= x)) {
			up++;
		}

		while((up < down) && (d_data[down] > x)){
			down--;
		}

		if(up == down){
			break;
		}

		distance_t tmp = d_data[up];
		d_data[up] = d_data[down];
		d_data[down] = tmp;

		label_t tmp2 = c_data[up];
		c_data[up] = c_data[down];
		c_data[down] = tmp2;

	}
	if(d_data[up] > x){
		up--;
	}

	d_data[lower] = d_data[up];
	d_data[up] = x;

	c_data[lower] = c_data[up];
	c_data[up] = x2;

	*pivot = up;
}

/**
 * @brief 
 * 
 * @param clusters 
 * @param sortData 
 * @param left 
 * @param right 
 */
void hdbscan_quicksort(ArrayList *clusters, ArrayList *sortData, int32_t left, int32_t right){

	distance_t *d_data = (distance_t *)sortData->data;
	label_t *c_data = (label_t *)clusters->data;
	if(left < right){
		int32_t pivot;
		partition(c_data, d_data, left, right, &pivot);
		hdbscan_quicksort(clusters, sortData, left, pivot - 1);
		hdbscan_quicksort(clusters, sortData, pivot + 1, right);
	}
}

/**
 * @brief Sorts the clusters using the distances in the distanceMap. This
 * function requires that the confidences be already calculates. This
 * can be achieved by calling the hdbscan_calculate_stats function
 * 
 * @param distanceMap 
 * @param clusters 
 * @param distanceType 
 * @return ArrayList* 
 */
ArrayList* hdbscan_sort_by_similarity(hashtable* distanceMap, ArrayList *clusters, int32_t distanceType){
	
	assert(distanceMap != NULL); 
	ArrayList *distances;
	if(clusters == NULL){
		clusters = array_list_init(hashtable_size(distanceMap), sizeof(label_t), NULL);

		if(sizeof(label_t) == sizeof(int)) {
			clusters->compare = int_compare;
		} else if(sizeof(label_t) == sizeof(long)) {
			clusters->compare = long_compare;
		} else {
			clusters->compare = short_compare;
		}

		distances = array_list_init(hashtable_size(distanceMap), sizeof(distance_t), NULL);

	} else{
		distances = array_list_init(clusters->size, sizeof(distance_t), NULL);
	}
	
	if(sizeof(distance_t) == sizeof(double)) {
		distances->compare = double_compare;
	} else {
		distances->compare = float_compare;
	}


	int32_t size = clusters->size;

	if(size == 0){     /// If clusters had nothing in it, we will use the whole hash table

		label_t key;
		distance_values* dv = NULL;
		
		for(size_t i = 0; i < hashtable_size(distanceMap); i++)
		{
			key = ((label_t *)distanceMap->keys->data)[i];
			hashtable_lookup(distanceMap, &key, &dv);
		
			array_list_append(clusters, &key);
			distance_t conf;

			if(distanceType == CORE_DISTANCE_TYPE){
				conf = dv->cr_confidence;
			} else{
				conf = dv->dr_confidence;
			}

			array_list_append(distances, &conf);		
		}

	} else { /// else we just need to get the lengths from the hash table

		distances->size = clusters->size;
		distance_t *ddata = (distance_t *)distances->data;
		label_t key;
		distance_values *dv = NULL;
		distance_t conf;
#ifdef _OPENMP
#pragma omp parallel for private(key, dv, conf)
#endif
		for(int32_t i = 0; i < clusters->size; i++){
			key = ((label_t *)distanceMap->keys->data)[i];			
			hashtable_lookup(distanceMap, &key, &dv);
			
			if(distanceType == CORE_DISTANCE_TYPE){
				conf = dv->cr_confidence;
			} else{
				conf = dv->dr_confidence;
			}
			ddata[i] = conf;
		}
	}

	// sort
	hdbscan_quicksort(clusters, distances, 0, clusters->size-1);
	array_list_delete(distances);

	return clusters;
}

/**
 * @brief Sorts clusters according to how long the cluster is
 * 
 * @param clusterTable 
 * @param clusters 
 * @return ArrayList* 
 */
ArrayList* hdbscan_sort_by_length(hashtable* clusterTable, ArrayList *clusters){

	assert(clusterTable != NULL && clusters != NULL);
	ArrayList *lengths;
	if(clusters == NULL){
		clusters = array_list_init(hashtable_size(clusterTable), sizeof(label_t), NULL);

		if(sizeof(label_t) == sizeof(int)) {
			clusters->compare = int_compare;
		} else if(sizeof(label_t) == sizeof(long)) {
			clusters->compare = long_compare;
		} else {
			clusters->compare = short_compare;
		}

		lengths = array_list_init(hashtable_size(clusterTable), sizeof(distance_t), NULL);
	} else{
		lengths = array_list_init(clusters->size, sizeof(distance_t), NULL);
	}
	
	if(sizeof(distance_t) == sizeof(double)) {
		lengths->compare = double_compare;
	} else {
		lengths->compare = float_compare;
	}
	
	int32_t size = clusters->size;

	if(size == 0){     /// If clusters had nothing in it, we will use the whole hash table
		label_t key;
		for(size_t i = 0; i < hashtable_size(clusterTable); i++)
		{
			ArrayList *lst = NULL;
			key = ((label_t *)clusterTable->keys)[i];
			hashtable_lookup(clusterTable, &key, &lst);
			array_list_append(clusters, &key);
			distance_t tmp = (distance_t)lst->size;
			array_list_append(lengths, &tmp);
		}

	} else { /// else we just need to get the lengths from the hash table
		label_t *data = clusters->data;
		lengths->size = clusters->size;
#ifdef _OPENMP
#pragma omp parallel for
#endif
		for(int32_t i = 0; i < clusters->size; i++){
			label_t *key = data + i;
			ArrayList *lst = NULL;
			hashtable_lookup(clusterTable, key, &lst);
			distance_t tmp = (distance_t)lst->size;
			((distance_t *)lengths->data)[i] = tmp;
		}		
	}
	// sort
	hdbscan_quicksort(clusters, lengths, 0, clusters->size-1);
	array_list_delete(lengths);

	return clusters;
}

/**
 * @brief Destroys the cluster table
 * 
 * @param table 
 */
void hdbscan_destroy_cluster_map(hashtable* table){

	assert(table != NULL);
	hashtable_destroy(table, NULL, (void (*)(void *))array_list_delete);
	table = NULL;
}

/**
 * @brief 
 * 
 * @param table 
 */
void hdbscan_print_cluster_map(hashtable* table){

	assert(table != NULL);

	label_t key;
	ArrayList *clusterList = NULL;
	for(index_t i = 0; i < hashtable_size(table); i++)
	{		
		key = ((label_t *)table->keys->data)[i];
		hashtable_lookup(table, &key, &clusterList);
		printf("%d -> [", key);
		index_t dpointer;

		for(index_t j = 0; j < clusterList->size; j++){
			
			dpointer = ((label_t *)clusterList->data)[j];
			printf("%d ", dpointer);
		}
		printf("]\n");
	}
}

/**
 * @brief 
 * 
 * @param table 
 */
void hdbscan_print_cluster_sizes(hashtable* table){

	assert(table != NULL);
	label_t key;
	ArrayList *clusterList = NULL;
	for(size_t i = 0; i < hashtable_size(table); i++)
	{	
		key = ((label_t *)table->keys->data)[i];
		hashtable_lookup(table, &key, &clusterList);
		printf("%d : %ld\n", key, clusterList->size);
	}
}

/**
 * @brief 
 * 
 * @param hierarchy 
 * @param numPoints 
 * @param filename 
 */
void hdbscan_print_hierarchies(hashtable* hierarchy, uint numPoints, char *filename){

	assert(hierarchy != NULL);
	FILE *visFile = NULL;
	FILE *hierarchyFile = NULL;

	/// TODO: Use strcat_s
	if(filename != NULL){

		char visFilename[100];
		strcat(visFilename, filename);
		strcat(visFilename, "_visualization.vis");
		visFile = fopen(visFilename, "w");
		fprintf(visFile, "1\n");
		fprintf(visFile, "%ld\n", hashtable_size(hierarchy));
		fclose(visFile);

		char hierarchyFilename[100];
		strcat(hierarchyFilename, filename);
		strcat(hierarchyFilename, "_hierarchy.csv");
		hierarchyFile = fopen(hierarchyFilename, "w");
	}

	printf("\n////////////////////////////////////////////////////// Printing Hierarchies //////////////////////////////////////////////////////\n");
	printf("hierarchy size = %ld\n", hashtable_size(hierarchy));
	
	for(size_t i = 0; i < set_size(hierarchy->keys); i++){
		int64_t level;
		hierarchy_entry* data;
		set_value_at(hierarchy->keys, i, &level);
        hashtable_lookup(hierarchy, &level, &data);

		if(hierarchyFile){
			fprintf(hierarchyFile, "%.15f,", data->edgeWeight);
		} else {
			printf("%ld : %.15f -> [", level, data->edgeWeight);
		}

		for(int j = 0; j < numPoints; j++){
			if(hierarchyFile){
				fprintf(hierarchyFile, "%d,", data->labels[j]);
			} else {
				printf("%d ", data->labels[j]);
			}
		}

		if(hierarchyFile){
			fprintf(hierarchyFile, "\n");
		} else {
			printf("]\n");
		}
	}

	if(hierarchyFile){
		fclose(hierarchyFile);
	}
	printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
}

/**
 * @brief 
 * 
 * @param distancesMap 
 */
void hdbscan_print_distance_map(hashtable* distancesMap){

	printf("\n/////////////////////////////////////////////////////// Printing Distances ///////////////////////////////////////////////////////\n");
	for(size_t i = 0; i < hashtable_size(distancesMap); i++)
	{
		label_t key;
		distance_values* dv = NULL;
		set_value_at(distancesMap->keys, i, &key);
		hashtable_lookup(distancesMap, &key, &dv);
		printf("%d -> {\n", key);
		printf("\tmin_cr : %f, max_cr : %f, cr_confidence : %f\n", dv->min_cr, dv->max_cr, dv->cr_confidence);
		printf("\tmin_dr : %f, max_dr : %f, dr_confidence : %f\n", dv->min_dr, dv->max_dr, dv->dr_confidence);

		printf("}\n\n");
	}
	printf("///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
}

/**
 * \brief Print statistics
 * 
 * \param stats 
 */
void hdbscan_print_stats(clustering_stats* stats){

	printf("////////////////////////////////////////////////////// Statistical Values ////////////////////////////////////////////////////////\n");

	printf("Cluster Count 					: %d\n", stats->count);
	printf("Core Distances - Max 			: %.5f\n", stats->coreDistanceValues.max);
	printf("Core Distances - Mean 			: %.5f\n", stats->coreDistanceValues.mean);
	printf("Core Distances - Skewness 		: %.5f\n", stats->coreDistanceValues.skewness);
	printf("Core Distances - Kurtosis 		: %.5f\n", stats->coreDistanceValues.kurtosis);
	printf("Core Distances - Variance 		: %.5f\n", stats->coreDistanceValues.variance);
	printf("Core Distances - Standard Dev 	: %.5f\n\n", stats->coreDistanceValues.standardDev);

	printf("Intra Distances - Max 			: %.5f\n", stats->intraDistanceValues.max);
	printf("Intra Distances - Mean 			: %.5f\n", stats->intraDistanceValues.mean);
	printf("Intra Distances - Skewness 		: %.5f\n", stats->intraDistanceValues.skewness);
	printf("Intra Distances - Kurtosis 		: %.5f\n", stats->intraDistanceValues.kurtosis);
	printf("Intra Distances - Variance 		: %.5f\n", stats->intraDistanceValues.variance);
	printf("Intra Distances - Standard Dev 	: %.5f\n", stats->intraDistanceValues.standardDev);

	printf("//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n");
}

/**
 * \brief Destroy a hierarchy entry
 * 
 * \param entry 
 */
void hdbscan_destroy_hierarchical_entry(hierarchy_entry* entry)
{
	if(entry)
	{
		free(entry->labels);
		free(entry);
	}
}