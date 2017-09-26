/*
 * hdbscan.c
 *
 *  Created on: 13 Sep 2017
 *      Author: ojmakh
 */

#include "hdbscan/hdbscan.h"

uint hdbscan_get_dataset_size(uint rows, uint cols, boolean rowwise){
    if(rowwise == 1){
        return rows;
    } else{
        return rows * cols;
    }
}

/**
 * Removes the set of points from their parent Cluster, and creates a new Cluster, provided the
 * clusterId is not 0 (noise).
 * @param points The set of points to be in the new Cluster
 * @param clusterLabels An array of cluster labels, which will be modified
 * @param parentCluster The parent Cluster of the new Cluster being created
 * @param clusterLabel The label of the new Cluster
 * @param edgeWeight The edge weight at which to remove the points from their previous Cluster
 */
cluster* hdbscan_create_new_cluster(hdbscan* sc, IntSet* points, int32_t* clusterLabels, cluster* parentCluster, int32_t clusterLabel, double edgeWeight){

	ListNode* node = g_list_first(points);

	while(node != NULL){
		int32_t* it = node->data;

		if(it != NULL){
			clusterLabels[*it] = clusterLabel;
		}

		node = g_list_next(node);
	}

	guint ptsSize = g_list_length(points);

	cluster_detach_points(parentCluster, ptsSize, edgeWeight);

	if (clusterLabel != 0) {

		cluster* new = cluster_init(NULL, clusterLabel, parentCluster, edgeWeight, ptsSize);
		return new;
	} else{
		cluster_add_points_to_virtual_child_cluster(parentCluster, points);
		return NULL;
	}

}


hdbscan* hdbscan_init(hdbscan* sc, uint minPoints, uint datatype){
	if(sc == NULL)
		sc = (hdbscan*) malloc(sizeof(hdbscan));

	if(sc == NULL){
		printf("Error: Could not allocate memory for HDBSCAN.\n");
	} else{
		sc->minPoints = minPoints;
		distance_init(&sc->distanceFunction, _EUCLIDEAN, datatype);

		sc->selfEdges = TRUE;

		sc->hierarchy = g_hash_table_new(g_int64_hash, g_int64_equal);
		sc->clusterStabilities = g_hash_table_new(g_int_hash, g_int_equal);

		sc->dataSet = NULL;
		sc->constraints = NULL;
		sc->clusterLabels = NULL;
		sc->clusters = NULL;
		sc->coreDistances = NULL;
		sc->outlierScores = NULL;
		sc->clusterTable = NULL;

	}
	return sc;
}

void hdbscan_clean(hdbscan* sc){

	if(sc->clusterLabels != NULL){
		free(sc->clusterLabels);
	}
	
	if(sc->outlierScores != NULL){
		free(sc->outlierScores);
	}

	distance_clean(&sc->distanceFunction);
	
	graph_clean(&sc->mst);

	if(sc->constraints != NULL){

		ListNode* node = g_list_first(sc->constraints);

		while(node != NULL){
			constraint* c = node->data;
			constraint_destroy(c);
			node = g_list_next(node);
		}
	}

	if(sc->clusterStabilities != NULL){

		DoubleList *values = g_hash_table_get_values(sc->clusterStabilities);

		ListNode* node = g_list_first(values);

		while(node != NULL){

			DoubleList* stabilities = node->data;
			g_list_free_full(stabilities, (GDestroyNotify)free);
			node = g_list_next(node);
		}
		g_list_free_full(values, (GDestroyNotify)free);

		g_hash_table_destroy(sc->clusterStabilities);

	}


	if(sc->hierarchy != NULL){

		GHashTableIter iter;
		gpointer key;
		gpointer value;
		g_hash_table_iter_init (&iter, sc->hierarchy);

		while (g_hash_table_iter_next (&iter, &key, &value)){
			int32_t* label = (int32_t*)value;
			if(label != NULL)
				free(label);
			if(key != NULL)
				free(key);
		}
		g_hash_table_destroy(sc->hierarchy);

	}
	
	if(sc->clusterTable != NULL){

		hdbscan_destroy_cluster_table(sc->clusterTable);
		sc->clusterTable = NULL;

	}

	if(sc->clusters != NULL){

		for(guint i = 0; i < sc->clusters->len; i++){
			cluster* cl = (sc->clusters->pdata)[i];
			cluster_destroy(cl);
		}

		g_ptr_array_free(sc->clusters, TRUE);
	}

}

void hdbscan_destroy(hdbscan* sc){
	hdbscan_clean(sc);
	if(sc != NULL){
		free(sc);
	}
}


int32_t hdbscan_run(hdbscan* sc, void* dataset, uint rows, uint cols, boolean rowwise){

	if(sc == NULL){
		printf("hdbscan_run: sc has not been initialised.\n");
		return HDBSCAN_ERROR;
	}

	//printf("hdbscan_run: Running hdbscan\n");
	sc->numPoints = hdbscan_get_dataset_size(rows, cols, rowwise);
	//printf("hdbscan_run: numPoints set as %d\n", sc->numPoints);
	guint csize = sc->numPoints/2;
	sc->clusters = g_ptr_array_sized_new(csize);

	distance_compute(&sc->distanceFunction, dataset, rows, cols, sc->minPoints-1);
	//printf("hdbscan_run: distance computed\n");

	int32_t err = hdbscan_construct_mst(sc);
	if(err == HDBSCAN_ERROR){
		printf("Error: Could not construct the minimum spanning tree.\n");
		return HDBSCAN_ERROR;
	}

	//printf("hdbscan_run: minimum spanning tree created\n");
	graph_quicksort_by_edge_weight(&sc->mst);
	//graph_print(sc->mst);
	//printf("hdbscan_run: weights sorted\n");

	double pointNoiseLevels[sc->numPoints];
	int32_t pointLastClusters[sc->numPoints];

	hdbscan_compute_hierarchy_and_cluster_tree(sc, 0, pointNoiseLevels, pointLastClusters);
	//printf("hdbscan_run: hierarchy and cluster tree computed\n");
	int32_t infiniteStability = hdbscan_propagate_tree(sc);
	//printf("hdbscan_run: tree propagated\n");
	hdbscan_find_prominent_clusters(sc, infiniteStability);
	//printf("hdbscan_run: prominent clusters found\n");

	return HDBSCAN_SUCCESS;
}



/**
 * Calculates the number of constraints satisfied by the new clusters and virtual children of the
 * parents of the new clusters.
 * @param newClusterLabels Labels of new clusters
 * @param clusters An vector of clusters
 * @param constraints An vector of constraints
 * @param clusterLabels an array of current cluster labels for points
 */
void hdbscan_calculate_num_constraints_satisfied(hdbscan* sc, IntSet* newClusterLabels,	int32_t* currentClusterLabels){

	if(g_list_length(sc->constraints) == 0){
		return;
	}

}

int32_t hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int32_t compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters){

	int64_t lineCount = 0; // Indicates the number of lines written into
								// hierarchyFile.

	//The current edge being removed from the MST:
	int32_t currentEdgeIndex = sc->mst.esize - 1;

	int32_t nextClusterLabel = 2;
	boolean nextLevelSignificant = TRUE;
	//The previous and current cluster numbers of each point in the data set:
	int32_t numVertices = sc->mst.numVertices;
	int32_t previousClusterLabels[numVertices];
	int32_t currentClusterLabels[numVertices];

	for(int32_t i = 0; i < numVertices; i++){
		previousClusterLabels[i] = 1;
		currentClusterLabels[i] = 1;
	}

	//A list of clusters in the cluster tree, with the 0th cluster (noise) null:
	g_ptr_array_add(sc->clusters, NULL);

	//clusters.push_back(new Cluster(1, NULL, NAN, mst.getNumVertices()));
	cluster* c = cluster_init(NULL, 1, NULL, NAN, numVertices);
	g_ptr_array_add(sc->clusters, c);
	
	//Sets for the clusters and vertices that are affected by the edge(s) being removed:
	IntSet* affectedClusterLabels = NULL;
	guint affectedLabelsSize = 0;
	IntSet* affectedVertices = NULL;
	guint affectedVerticesSize = 0;
	//int32_t dd = 0;
	while (currentEdgeIndex >= 0) {

		double currentEdgeWeight = sc->mst.edgeWeights[currentEdgeIndex];
		//int32_t cc = 0;
		ClusterList* newClusters = NULL;
		//Remove all edges tied with the current edge weight, and store relevant clusters and vertices:

		while(currentEdgeIndex >= 0 && sc->mst.edgeWeights[currentEdgeIndex] == currentEdgeWeight){
			int32_t firstVertex = sc->mst.verticesA[currentEdgeIndex];

			int32_t secondVertex = sc->mst.verticesB[currentEdgeIndex];

			graph_remove_edge(&sc->mst, firstVertex, secondVertex);

			if (currentClusterLabels[firstVertex] == 0) {
				currentEdgeIndex--;
				continue;
			}

			affectedVertices = set_int_insert(affectedVertices, firstVertex, &affectedVerticesSize);
			affectedVertices = set_int_insert(affectedVertices, secondVertex, &affectedVerticesSize);

			affectedClusterLabels = set_int_insert(affectedClusterLabels, currentClusterLabels[firstVertex], &affectedLabelsSize);

			currentEdgeIndex--;
		}


		//Check each cluster affected for a possible split:
		while(affectedLabelsSize > 0){
			ListNode* it = g_list_last(affectedClusterLabels);

			int32_t examinedClusterLabel = *((int32_t *)it->data);
			affectedClusterLabels = list_full_link_delete(affectedClusterLabels, it, free);
			affectedLabelsSize--;

			IntSet* examinedVertices = NULL;
			guint examinedSize = 0;

			//Get all affected vertices that are members of the cluster currently being examined:
			it = g_list_first(affectedVertices);

			while(it != NULL){
				int32_t n = *((int32_t *)it->data);
				if (currentClusterLabels[n] == examinedClusterLabel) {
					examinedVertices = set_int_insert(examinedVertices, n, &examinedSize);

					ListNode* tmp = g_list_next(it);
					affectedVertices = list_full_link_delete(affectedVertices, it, free);
					affectedVerticesSize--;
					it = tmp;
				} else{
					it = g_list_next(it);
				}
			}

			IntSet* firstChildCluster = NULL;
			guint firstChildSize = 0;

			IntList* unexploredFirstChildClusterPoints = NULL;
			guint unexploredFirstChildSize = 0;

			//guint unexploredSize = g_list_length(unexploredFirstChildClusterPoints); // Use this to track the size of the list without having to call the O(n)operation of length
			int32_t numChildClusters = 0;

			/* Check if the cluster has split or shrunk by exploring the graph from each affected
			 * vertex.  If there are two or more valid child clusters (each has >= minClusterSize
			 * points), the cluster has split.
			 * Note that firstChildCluster will only be fully explored if there is a cluster
			 * split, otherwise, only spurious components are fully explored, in order to label
			 * them noise.
			 */
			while (examinedSize > 0) {
				//TODO Clean up this
				IntSet* constructingSubCluster = NULL;
				guint constructingSize = 0; //g_list_length(constructingSubCluster);

				IntList* unexploredSubClusterPoints = NULL;
				guint unexploredSubClusterPointsSize = 0;

				boolean anyEdges = FALSE;
				boolean incrementedChildCount = FALSE;

				ListNode* itr = g_list_last(examinedVertices);
				int32_t rootVertex = *((int32_t *)itr->data);

				constructingSubCluster = set_int_insert(constructingSubCluster, rootVertex, &constructingSize);

				unexploredSubClusterPoints = list_int_insert(unexploredSubClusterPoints, rootVertex);
				unexploredSubClusterPointsSize++;

				examinedVertices = list_full_link_delete(examinedVertices, itr, free);
				examinedSize--;
				//Explore this potential child cluster as long as there are unexplored points:
				while (unexploredSubClusterPointsSize > 0) {
					itr = g_list_first(unexploredSubClusterPoints);
					int32_t vertexToExplore = *((int32_t *)itr->data);
					unexploredSubClusterPoints = list_full_link_delete(unexploredSubClusterPoints, itr, free);
					unexploredSubClusterPointsSize--;

					IntList* v = sc->mst.edges[vertexToExplore];

					itr = g_list_first(v);
					while(itr != NULL){
						if(itr->data != NULL){
							int32_t neighbor = *((int32_t *)itr->data);
							anyEdges = TRUE;

							int32_t s1 = constructingSize; 
							constructingSubCluster = set_int_insert(constructingSubCluster, neighbor, &constructingSize);
							// check that the insertion was successfully by comparing the previous and the current lengths
							if(s1 < constructingSize){
								unexploredSubClusterPoints = list_int_insert(unexploredSubClusterPoints, neighbor);
								unexploredSubClusterPointsSize++;

								ListNode* toDelete = g_list_find_custom(examinedVertices, &neighbor, (GCompareFunc)gint_compare);
								if(toDelete != NULL){
									examinedVertices = list_full_link_delete(examinedVertices, toDelete, free);
									examinedSize--;
								}
							}
						}

						itr = g_list_next(itr);
					}

					//Check if this potential child cluster is a valid cluster:
					if(incrementedChildCount == FALSE && constructingSize >= sc->minPoints && anyEdges == TRUE){
						incrementedChildCount = TRUE;
						numChildClusters++;

						//If this is the first valid child cluster, stop exploring it:
						if (firstChildSize == 0) {
							itr = g_list_first(constructingSubCluster);

							while(itr != NULL){
								int32_t d = *((int32_t*)itr->data);
								firstChildCluster = set_int_insert(firstChildCluster, d, &firstChildSize);

								itr = g_list_next(itr);
							}

							itr = g_list_first(unexploredSubClusterPoints);

							while(itr != NULL){
								int32_t d = *((int32_t*)itr->data);
								unexploredFirstChildClusterPoints = list_int_insert(unexploredFirstChildClusterPoints, d);
								unexploredFirstChildSize++;

								itr = g_list_next(itr);
							}
						}
					}
				}

				//If there could be a split, and this child cluster is valid:
				//guint constructingSize = g_list_length(constructingSubCluster);
				if(numChildClusters >= 2 && constructingSize >= sc->minPoints && anyEdges == TRUE){
					//Check this child cluster is not equal to the unexplored first child cluster:
					it = g_list_last(firstChildCluster);
					int32_t firstChildClusterMember = *((int32_t *)it->data);
					//if (constructingSubCluster.find(firstChildClusterMember) != constructingSubCluster.end()) {
					it = g_list_find_custom(constructingSubCluster, &firstChildClusterMember, gint_compare);
					if(it != NULL){
						numChildClusters--;
					}

					//Otherwise, create a new cluster:
					else {

						cluster* examinedCluster = (sc->clusters->pdata)[examinedClusterLabel]; //g_list_nth_data(sc->clusters, examinedClusterLabel);
						cluster* newCluster = hdbscan_create_new_cluster(sc, constructingSubCluster, currentClusterLabels, examinedCluster, nextClusterLabel, currentEdgeWeight);

						//printf("Otherwise, create a new cluster: %d of label %d\n", newCluster, newCluster->getLabel());
						newClusters = g_list_append(newClusters, newCluster);
						nextClusterLabel++;

						g_ptr_array_add(sc->clusters, newCluster);

					}
				}

				//If this child cluster is not valid cluster, assign it to noise:
				else if(constructingSize < sc->minPoints || anyEdges == FALSE){
					cluster* examinedCluster = (sc->clusters->pdata)[examinedClusterLabel];

					cluster* newCluster = hdbscan_create_new_cluster(sc, constructingSubCluster, currentClusterLabels, examinedCluster, 0, currentEdgeWeight);

					it = g_list_first(constructingSubCluster);
					while(it != NULL){
						if(it->data != NULL){
							int32_t point = *((int32_t*)it->data);
							pointNoiseLevels[point] = currentEdgeWeight;
							pointLastClusters[point] = examinedClusterLabel;
						}
						it = g_list_next(it);
					}

					cluster_destroy(newCluster);

				}


				/********************
				 * Clean up constructing subcluster
				 *******************/
				list_int_clean(constructingSubCluster);
				list_int_clean(unexploredSubClusterPoints);
			}

			//Finish exploring and cluster the first child cluster if there was a split and it was not already clustered:
			ListNode *p = g_list_first(firstChildCluster);
			if (numChildClusters >= 2 && p != NULL && currentClusterLabels[*((int32_t *)p->data)] == examinedClusterLabel) {
				//guint unexploredSize = g_list_length(unexploredFirstChildClusterPoints);
				while(unexploredFirstChildSize > 0){
					it = g_list_first(unexploredFirstChildClusterPoints);
					int32_t vertexToExplore = *((int32_t *)it->data);

					unexploredFirstChildClusterPoints = list_full_link_delete(unexploredFirstChildClusterPoints, it, free);
					unexploredFirstChildSize--;

					IntList* v = sc->mst.edges[vertexToExplore];
					ListNode* itr = g_list_first(v);
					while(itr != NULL){
						int32_t neighbor = *((int32_t *)itr->data);

						int32_t s1 = firstChildSize; //g_list_length(firstChildCluster);
						firstChildCluster = set_int_insert(firstChildCluster, neighbor, &firstChildSize);
						if (s1 < firstChildSize){//g_list_length(firstChildCluster)) {
							unexploredFirstChildClusterPoints = list_int_insert(unexploredFirstChildClusterPoints, neighbor);
							unexploredFirstChildSize++;
						}
						itr = g_list_next(itr);
					}
				}

				cluster* examinedCluster =(sc->clusters->pdata)[examinedClusterLabel];// g_list_nth_data(sc->clusters, examinedClusterLabel);
				cluster* newCluster = hdbscan_create_new_cluster(sc, firstChildCluster, currentClusterLabels, examinedCluster, nextClusterLabel, currentEdgeWeight);

				newClusters = g_list_append(newClusters, newCluster);
				nextClusterLabel++;

				g_ptr_array_add(sc->clusters, newCluster);
			}
			list_int_clean(firstChildCluster);
			list_int_clean(unexploredFirstChildClusterPoints);
			list_int_clean(examinedVertices);
		}

		if (compactHierarchy == FALSE || nextLevelSignificant == TRUE || g_list_length(newClusters) > 0) {
			lineCount++;
			int32_t* newpl = (int32_t*)malloc(numVertices * sizeof(int32_t));
			memcpy(newpl, previousClusterLabels, numVertices * sizeof(int32_t));

			int64_t *l = (int64_t *) malloc(sizeof(int64_t));
			*l = lineCount;
			g_hash_table_insert(sc->hierarchy, l, newpl);
		}

		// Assign file offsets and calculate the number of constraints
					// satisfied:
		IntSet* newClusterLabels = NULL;
		guint newClusterLabelsSize = 0;
		ListNode* itr = g_list_first(newClusters);
		while(itr != NULL){
			cluster* newCluster = itr->data;

			newCluster->offset = lineCount;
			newClusterLabels = set_int_insert(newClusterLabels, lineCount, &newClusterLabelsSize);

			itr = g_list_next(itr);
		}

		if (newClusterLabelsSize > 0){
			//calculateNumConstraintsSatisfied(newClusterLabels, currentClusterLabels);
		}

		for (uint i = 0; i < numVertices; i++) {
			previousClusterLabels[i] = currentClusterLabels[i];
		}

		if (g_list_length(newClusters) == 0){
			nextLevelSignificant = FALSE;
		} else{
			nextLevelSignificant = TRUE;
		}

		g_list_free(newClusters);
		list_int_clean(newClusterLabels);
	}
	int32_t* labels = (int32_t*) malloc(numVertices * sizeof(int32_t));
	memset(labels, 0, numVertices * sizeof(int32_t));
	// Write out the final level of the hierarchy (all points noise):

	int64_t *l = (int64_t *) malloc(sizeof(int64_t));
	*l = 0;
	g_hash_table_insert(sc->hierarchy, l, labels);
	lineCount++;

	list_int_clean(affectedClusterLabels);
	list_int_clean(affectedVertices);

	return HDBSCAN_SUCCESS;
}

void print_distances(hdbscan* sc){
	for (int32_t i = 0; i < sc->numPoints; i++) {
		printf("[");
		for (int32_t j = 0; j < sc->numPoints; j++) {
			printf("%f ", distance_get(&sc->distanceFunction, i, j));
		}

		printf("]\n");
	}
	printf("\n");
}

void print_graph_components(int32_t *nearestMRDNeighbors, int32_t *otherVertexIndices, double *nearestMRDDistances, int32_t ssize){
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>nearestMRDNeighbors>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	for(int32_t i = 0; i < ssize; i++){
		printf("%d ", nearestMRDNeighbors[i]);
	}

	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>otherVertexIndices>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	for(int32_t i = 0; i < ssize; i++){
		printf("%d ", otherVertexIndices[i]);

	}

	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>nearestMRDDistances>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	for(int32_t i = 0; i < ssize; i++){
		printf("%f \n", nearestMRDDistances[i]);

	}

	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n\n\n");
}

int32_t hdbscan_construct_mst(hdbscan* sc){
	double*  coreDistances = sc->distanceFunction.coreDistances;

	int32_t selfEdgeCapacity = 0;
	uint size = sc->numPoints;
	if (sc->selfEdges == TRUE){
		//printf("Self edges set to true\n");
		selfEdgeCapacity = size;
	}

	//One bit is set (true) for each attached point, or unset (false) for unattached points:
	boolean attachedPoints[size];

	for(int32_t i = 0; i < size-1; i++){
		attachedPoints[i] = FALSE;
	}

	//The MST is expanded starting with the last point in the data set:
	int32_t currentPoint = size - 1;
	attachedPoints[size - 1] = TRUE;

	//Each point has a current neighbor point in the tree, and a current nearest distance:
	int32_t ssize = size - 1 + selfEdgeCapacity;
	int32_t *nearestMRDNeighbors = (int32_t *) malloc(ssize * sizeof(int32_t));

	//Create an array for vertices in the tree that each point attached to:
	int32_t *otherVertexIndices = (int32_t *) malloc(ssize * sizeof(int32_t));

	double *nearestMRDDistances = (double *) malloc(ssize * sizeof(double));//, numeric_limits<double>::max());

	for(int32_t i = 0; i < ssize; i++){
		nearestMRDDistances[i] = DBL_MAX;
		nearestMRDNeighbors[i] = 0;
		otherVertexIndices[i] = 0;
	}


//#pragma omp parallel for
	//Continue attaching points to the MST until all points are attached:
	for (uint numAttachedPoints = 1; numAttachedPoints < size; numAttachedPoints++) {
		int32_t nearestMRDPoint = -1;
		double nearestMRDDistance = DBL_MAX;

		//Iterate through all unattached points, updating distances using the current point:

		for (unsigned int neighbor = 0; neighbor < size; neighbor++) {
			
			if (currentPoint == neighbor) {
				continue;
			}

			if (attachedPoints[neighbor] == TRUE) {
				continue;
			}

			double mutualReachabiltiyDistance = distance_get(&sc->distanceFunction, neighbor, currentPoint);
			
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
		attachedPoints[nearestMRDPoint] = TRUE;
		otherVertexIndices[numAttachedPoints] = numAttachedPoints;
		currentPoint = nearestMRDPoint;
		
	}

	//If necessary, attach self edges:
	if (sc->selfEdges == TRUE) {
//#ifdef USE_OPENMP
//#pragma omp parallel for
//#endif
		for (uint i = size - 1; i < size * 2 - 1; i++) {
			int32_t vertex = i - (size - 1);
			nearestMRDNeighbors[i] = vertex;
			otherVertexIndices[i] = vertex;
			nearestMRDDistances[i] = coreDistances[vertex];
			//printf("At %d coreDistances[%d] = %f\n", i, vertex, coreDistances[vertex]);
		}
	}


	graph_init(&sc->mst, size, nearestMRDNeighbors, ssize, otherVertexIndices, ssize, nearestMRDDistances, ssize);
	//graph_print(sc->mst);

	if(&sc->mst == NULL){
		printf("Error: Could not initialise mst.\n");
		return HDBSCAN_ERROR;
	}

	return HDBSCAN_SUCCESS;
}

boolean hdbscan_propagate_tree(hdbscan* sc){

	IntSet* clustersToExamine = NULL;
	guint clustersToExamineSize = 0;
	boolean addedToExaminationList[sc->clusters->len];
	boolean infiniteStability = FALSE;
	for(int32_t i = 0; i < sc->clusters->len; i++){
		addedToExaminationList[i] = FALSE;
	}

	for(guint i = 0; i < sc->clusters->len; i++){

		cluster* cl = (sc->clusters->pdata)[i];

		if(cl != NULL && cl->hasChildren == FALSE){
			clustersToExamine = set_int_insert(clustersToExamine, cl->label, &clustersToExamineSize);
			addedToExaminationList[cl->label] = TRUE;
		}
	}

	while(clustersToExamineSize > 0){
		ListNode* last = g_list_last(clustersToExamine);
		int x = *((int32_t *)last->data);
		cluster* currentCluster = (cluster*)(sc->clusters->pdata)[x];
		clustersToExamine = list_full_link_delete(clustersToExamine, last, free);
		clustersToExamineSize--;

		//printf("Examining cluster %d\n", currentCluster->label);
		cluster_propagate(currentCluster);
		if(currentCluster->stability == DBL_MAX){
			infiniteStability = TRUE;
		}
		if(currentCluster->parent != NULL){
			cluster *parent = currentCluster->parent;

			if(addedToExaminationList[parent->label] == FALSE){
				clustersToExamine = set_int_insert(clustersToExamine, parent->label, &clustersToExamineSize);
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
	g_list_free_full(clustersToExamine, free);
	return infiniteStability;
}

void hdbscan_find_prominent_clusters(hdbscan* sc, int32_t infiniteStability){
	cluster* cl = (cluster *)(sc->clusters->pdata)[1];
	ClusterList *solution = cl->propagatedDescendants;

	LongIntListMap *significant = g_hash_table_new(g_int64_hash, g_int64_equal);
	ListNode* node = g_list_first(solution);

	int x = 0;
	while(node != NULL){
		cluster* c = (cluster *)node->data;

		if(c != NULL){
			IntList* clusterList = (IntList *)g_hash_table_lookup(significant, &c->offset);

			clusterList = list_int_insert(clusterList, c->label);
			g_hash_table_insert(significant, &c->offset, clusterList);
		}
		x++;
		node = g_list_next(node);
	}
	
	sc->clusterLabels = (int32_t *)malloc(sc->numPoints * sizeof(int32_t));
	for (int32_t i = 0; i < sc->numPoints; i++) {
		(sc->clusterLabels)[i] = 0;
	}

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, significant);

	while (g_hash_table_iter_next (&iter, &key, &value)){
		int64_t offset = *((int64_t *)key);
		int64_t l = offset + 1;
		IntList* clusterList = (IntList*)value;
		int32_t* hpSecond = (int32_t *)g_hash_table_lookup(sc->hierarchy, &l);

		for(int32_t i = 0; i < sc->numPoints; i++){
			int32_t label = hpSecond[i];
			ListNode* it = g_list_find_custom(clusterList, &label, gint_compare);
			if(it != NULL){
				sc->clusterLabels[i] = label;
			}
		}
	}
	g_hash_table_iter_init (&iter, significant);

	while (g_hash_table_iter_next (&iter, &key, &value)){

		IntList* clusterList = (IntList*)value;
		list_int_clean(clusterList);
	}

	g_hash_table_destroy(significant);
}



int hdbscsan_calculate_outlier_scores(hdbscan* sc, double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability){


	double* coreDistances = sc->distanceFunction.coreDistances;
	int numPoints = sc->numPoints;
	sc->outlierScores = (outlier_score*)malloc(numPoints*sizeof(outlier_score));

	if(!sc->outlierScores){
		printf("ERROR: Calculate Outlier Score - Could not allocate memory for outlier scores.\n");
		return HDBSCAN_ERROR;
	}

	int i = 0;
#pragma omp parallel for	
	for(uint i = 0; i < sc->numPoints; i++){
		int tmp = pointLastClusters[i];
		cluster* c = (cluster*)(sc->clusters->pdata)[tmp];
		double epsilon_max = c->propagatedLowestChildDeathLevel;
		double epsilon = pointNoiseLevels[i];
		
		double score = 0;
		if(epsilon != 0){
			score = 1 - (epsilon_max / epsilon);
		}
		
		sc->outlierScores[i].id = i;		
		sc->outlierScores[i].score = score;
		sc->outlierScores[i].coreDistance = coreDistances[i];
	}

	//Sort the outlier scores:
	qsort(sc->outlierScores, numPoints, sizeof(outlier_score), outlier_score_compare);
}

IntIntListMap* hdbscan_create_cluster_table(int32_t* labels, int32_t size){
	IntIntListMap* clusterTable = g_hash_table_new(g_int_hash, g_int_equal);
	
	for(int32_t i = 0; i < size; i++){
		int32_t *label = (int32_t *)malloc(sizeof(int32_t));
		*label = labels[i];
		IntPtrList* clusterList = (IntPtrList *)g_hash_table_lookup(clusterTable, label);
		
		if(clusterList == NULL){
			clusterList = g_ptr_array_new_full(size, free);
		}
		list_int_ptr_insert(clusterList, i);
		g_hash_table_insert(clusterTable, label, clusterList);
	}
	
	return clusterTable;
}

void hdbscan_destroy_cluster_table(IntIntListMap* table){
	
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, table);

	while (g_hash_table_iter_next (&iter, &key, &value)){
		int32_t label = *((int32_t *)key);
		IntPtrList* clusterList = (IntPtrList*)value;						
		g_ptr_array_free(clusterList, TRUE);
			
		if(key != NULL){
			free(key);
		}
	}
	g_hash_table_destroy(table);
}
