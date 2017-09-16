/*
 * hdbscan.h
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef HDBSCAN_H_
#define HDBSCAN_H_
#include <glib.h>
#include <float.h>

#include "cluster.h"
#include "constraint.h"
#include "distance.h"
#include "outlier_score.h"
#include "undirected_graph.h"

#define FILE_BUFFER_SIZE = 32678
#define HDBSCAN_SUCCESS 1
#define HDBSCAN_ERROR	0

/**
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 * @author zjullion
 */

//string getWarningMessage();

typedef struct HDBSCAN {
	distance* distanceFunction = NULL;
	double* dataSet = NULL;
	UndirectedGraph* mst = NULL;
	constraint* constraints = NULL;
	double* coreDistances = NULL;
	GList* clusters = NULL;
	outlier_score* outlierScores = NULL;
	int* clusterLabels = NULL;
	GHashTable* hierarchy = NULL;
	GHashTable *clusterStabilities = NULL;
	int selfEdges = 1;
	uint minPoints, minClusterSize, numPoints;
} hdbscan;

int hdbscan_init(hdbscan* h);

void hdbscan_destroy(hdbscan* h);

/**
 *
 */
int hdbscan_run(hdbscan* sc, double* dataset, uint rows, uint cols, int rowwise);
int hdbscan_run(hdbscan* sc, double* dataset, int size);
int hdbscan_run(hdbscan* sc);

/**
 *
 */
int hdbscan_construct_mst(hdbscan* sc);

/**
 * Computes the hierarchy and cluster tree from the minimum spanning tree, writing both to file,
 * and returns the cluster tree.  Additionally, the level at which each point becomes noise is
 * computed.  Note that the minimum spanning tree may also have self edges (meaning it is not
 * a true MST).
 * @param mst A minimum spanning tree which has been sorted by edge weight in descending order
 * @param minClusterSize The minimum number of points which a cluster needs to be a valid cluster
 * @param compactHierarchy Indicates if hierarchy should include all levels or only levels at
 * which clusters first appear
 * @param constraints An optional vector of Constraints to calculate cluster constraint satisfaction
 * @param hierarchyOutputFile The path to the hierarchy output file
 * @param treeOutputFile The path to the cluster tree output file
 * @param delimiter The delimiter to be used while writing both files
 * @param pointNoiseLevels A vector<double> to be filled with the levels at which each point becomes noise
 * @param pointLastClusters An vector<int> to be filled with the last label each point had before becoming noise
 */
void hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int compactHierarchy, double* pointNoiseLevels, int* pointLastClusters);

/**
 * Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * @param clusters A list of Clusters forming a cluster tree
 * @return true if there are any clusters with infinite stability, false otherwise
 */
int hdbscan_propagate_tree(hdbscan* sc);

/**
 * Produces a flat clustering result using constraint satisfaction and cluster stability, and
 * returns an array of labels.  propagateTree() must be called before calling this method.
 * @param clusters A list of Clusters forming a cluster tree which has already been propagated
 * @param hierarchyFile The path to the hierarchy input file
 * @param flatOutputFile The path to the flat clustering output file
 * @param delimiter The delimiter for both files
 * @param numPoints The number of points in the original data set
 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
 */
void hdbscan_find_prominent_clusters(hdbscan* sc, int infiniteStability);
int* hdbscan_find_prominent_clusters(int infiniteStability, GList* solution);

#endif /* HDBSCAN_H_ */

