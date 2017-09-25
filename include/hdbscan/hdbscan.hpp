/*
 * hdbscan.h
 * Implementation of the hdbscan* algorithm, which is broken into several methods.
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef HDBSCAN_H_
#define HDBSCAN_H_
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "cluster.hpp"
#include "constraint.hpp"
#include "distance.hpp"
#include "outlier_score.hpp"
#include "undirected_graph.hpp"

using namespace std;
using namespace clustering;
using namespace clustering::distance;

#define FILE_BUFFER_SIZE = 32678

namespace clustering {
/**
 * Implementation of the hdbscan* algorithm, which is broken into several methods.
 * @author zjullion
 */

string getWarningMessage();

template <class T1>
class hdbscan {

private:
	DistanceCalculator<T1> distanceFunction;
	UndirectedGraph mst;
	vector<Constraint*> constraints;
	vector<Cluster*> clusters;
	vector<OutlierScore> outlierScores;
	vector<int> clusterLabels;
	map<int, double> clusterStabilities;
	map<long, vector<int> > hierarchy;
	bool selfEdges = true;
	uint minPoints, numPoints;

	/**
	 * Calculates the number of constraints satisfied by the new clusters and virtual children of the
	 * parents of the new clusters.
	 * @param newClusterLabels Labels of new clusters
	 * @param clusters An vector of clusters
	 * @param constraints An vector of constraints
	 * @param clusterLabels an array of current cluster labels for points
	 */
	void calculateNumConstraintsSatisfied(set<int>& newClusterLabels, vector<int>& currentClusterLabels);
	/**
	 * Removes the set of points from their parent Cluster, and creates a new Cluster, provided the
	 * clusterId is not 0 (noise).
	 * @param points The set of points to be in the new Cluster
	 * @param clusterLabels An array of cluster labels, which will be modified
	 * @param parentCluster The parent Cluster of the new Cluster being created
	 * @param clusterLabel The label of the new Cluster
	 * @param edgeWeight The edge weight at which to remove the points from their previous Cluster
	 */
	Cluster* createNewCluster(set<int>* points,
			vector<int>* clusterLabels, Cluster* parentCluster, int clusterLabel,
			double edgeWeight);

	void run();
public:
	hdbscan();
	hdbscan(calculator cal, uint minPts);
	~hdbscan();

	vector<Cluster*>& getClusters();
	vector<int>& getClusterLabels();
	map<int, double>& getClusterStabilities();

	/**
	 *
	 */
	void run(vector<T1>& dataset);
	void run(vector<T1>& dataset, int rows, int cols, bool rowwise);
	void run(T1* dataset, int size);
	void run(T1* dataset, int rows, int cols, bool rowwise);

	/**
	 * Calculates the core distances for each point in the data set, given some value for k.
	 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
	 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
	 * @param distanceFunction A DistanceCalculator to compute distances between points
	 */
	void calculateCoreDistances(T1* dataSet, int rows, int cols);
	void calculateCoreDistances(vector<vector<T1> >& dataSet);

	/**
	 * Constructs the minimum spanning tree of mutual reachability distances for the data set, given
	 * the core distances for each point.
	 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
	 * @param coreDistances An array of core distances for each data point
	 * @param selfEdges If each point should have an edge to itself with weight equal to core distance
	 * @param distanceFunction A DistanceCalculator to compute distances between points
	 */
	void constructMST();

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
	void computeHierarchyAndClusterTree(bool compactHierarchy,
			vector<double>* pointNoiseLevels, vector<int>* pointLastClusters);

	/**
	 * Propagates constraint satisfaction, stability, and lowest child death level from each child
	 * cluster to each parent cluster in the tree.  This method must be called before calling
	 * findProminentClusters() or calculateOutlierScores().
	 * @param clusters A list of Clusters forming a cluster tree
	 * @return true if there are any clusters with infinite stability, false otherwise
	 */
	bool propagateTree();

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
	void findProminentClusters(bool infiniteStability);
	vector<int> findProminentClusters(bool infiniteStability, vector<Cluster*>* solution);

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
	void calculateOutlierScores(vector<double>* pointNoiseLevels, vector<int>* pointLastClusters, bool infiniteStability);

	bool compareClusters(Cluster* one, Cluster* two);

	double* getCoreDistances();
	DistanceCalculator<T1>& getDistanceFunction();

	double getDistance(uint i, uint j);

	void clean();

// ------------------------------ PRIVATE METHODS ------------------------------





};
} /* namespace clustering */
#endif /* hdbscan_H_ */

