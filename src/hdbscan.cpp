/*
 * hdbscan.cpp
 *
 *  Created on: 22 Sep 2017
 *      Author: junior
 */


#ifdef __cplusplus
#include "hdbscan/hdbscan.hpp"
namespace clustering {


hdbscan::hdbscan(){
	hdbscan_init(this, 3);
}


hdbscan::hdbscan(uint minPts){
	hdbscan_init(this, minPts);
}

hdbscan::~hdbscan(){
	hdbscan_clean(this);
}

/**
 *
 */
void hdbscan::run(double* dataset, uint rows, uint cols, boolean rowwise){
	hdbscan_run(this, dataset, rows, cols, rowwise);
}

/**
 * Calculates the core distances for each point in the data set, given some value for k.
 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
 * @param distanceFunction A DistanceCalculator to compute distances between points
 */
//void hdbscan::calculateCoreDistances(double* dataSet, int rows, int cols);

/**
 * Constructs the minimum spanning tree of mutual reachability distances for the data set, given
 * the core distances for each point.
 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
 * @param coreDistances An array of core distances for each data point
 * @param selfEdges If each point should have an edge to itself with weight equal to core distance
 * @param distanceFunction A DistanceCalculator to compute distances between points
 */
void hdbscan::constructMST(){
	hdbscan_construct_mst(this);
}

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
void hdbscan::computeHierarchyAndClusterTree(boolean compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters){
	hdbscan_compute_hierarchy_and_cluster_tree(this, compactHierarchy, pointNoiseLevels, pointLastClusters);
}

/**
 * Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * @param clusters A list of Clusters forming a cluster tree
 * @return true if there are any clusters with infinite stability, false otherwise
 */
boolean hdbscan::propagateTree(){
	return hdbscan_propagate_tree(this);
}

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
void hdbscan::findProminentClusters(boolean infiniteStability){
	hdbscan_find_prominent_clusters(this, infiniteStability);
}

/**
 * Produces the outlier score for each point in the data set, and returns a sorted list of outlier
 * scores.  propagateTree() must be called before calling this method.
 * @param clusters A list of Clusters forming a cluster tree which has already been propagated
 * @param pointNoiseLevels A vector<double> with the levels at which each point became noise
 * @param pointLastClusters An vector<int> with the last label each point had before becoming noise
 * @param coreDistances An array of core distances for each data point
 * @param outlierScoresOutputFile The path to the outlier scores output file
 * @param delimiter The delimiter for the output file
 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
 */
void hdbscan::calculateOutlierScores(DoubleList* pointNoiseLevels, IntList* pointLastClusters, boolean infiniteStability){
	// TODO
}

void hdbscan::clean(){
	hdbscan_clean(this);
}
};
#endif
