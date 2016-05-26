/*
 * hdbscan.h
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef HDBSCAN_H_
#define HDBSCAN_H_
#include <cstdlib>
#include <vector>
#include <set>
#include <map>

#include "cluster.h"
#include "constraint.h"
#include "distance.h"
#include "outlier_score.h"
#include "undirected_graph.h"

using namespace std;
using namespace clustering;
using namespace clustering::distance;

#define FILE_BUFFER_SIZE = 32678

namespace clustering {
/**
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 * @author zjullion
 */

string getWarningMessage();

class HDBSCAN {

private:
	DistanceCalculator* distanceFunction;
	vector<vector<double> >* dataSet;
	UndirectedGraph* mst;
	vector<Constraint*>* constraints;
	vector<double>* coreDistances;
	vector<Cluster*>* clusters;
	vector<OutlierScore*>* outlierScores;
	vector<int>* clusterLabels;
	map<long, vector<int>* >* hierarchy;
	bool selfEdges = true;
	uint minPoints, minClusterSize, numPoints;

	/**
	 * Calculates the number of constraints satisfied by the new clusters and virtual children of the
	 * parents of the new clusters.
	 * @param newClusterLabels Labels of new clusters
	 * @param clusters An vector of clusters
	 * @param constraints An vector of constraints
	 * @param clusterLabels an array of current cluster labels for points
	 */
	void calculateNumConstraintsSatisfied(set<int> newClusterLabels, vector<int> currentClusterLabels);
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
public:

	HDBSCAN(calculator cal, uint minPoints, uint minClusterSize);
	HDBSCAN(vector<vector<double> >* dataSet, calculator cal, uint minPoints, uint minClusterSize);
	HDBSCAN(string fileName, calculator cal, uint minPoints, uint minClusterSize);
	HDBSCAN(string dataFileName, string constraintFileName, calculator cal, uint minPoints, uint minClusterSize);
	~HDBSCAN();

	vector<Cluster*>* getClusters();
	vector<int>* getClusterLabels();
	vector<vector<double> >* getDataSet();

	/**
	 * Reads in the input data set from the file given, assuming the delimiter separates attributes
	 * for each data point, and each point is given on a separate line.  Error messages are printed
	 * if any part of the input is improperly formatted.
	 * @param fileName The path to the input file
	 */
	void readInDataSet(string fileName);

	/**
	 *
	 */
	void run(bool useDataset);

	/**
	 * Reads in constraints from the file given, assuming the delimiter separates the points involved
	 * in the constraint and the type of the constraint, and each constraint is given on a separate
	 * line.  Error messages are printed if any part of the input is improperly formatted.
	 * @param fileName The path to the input file
	 */
	void readInConstraints(string fileName);

	/**
	 * Calculates the core distances for each point in the data set, given some value for k.
	 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
	 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
	 * @param distanceFunction A DistanceCalculator to compute distances between points
	 */
	void calculateCoreDistances();

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

// ------------------------------ PRIVATE METHODS ------------------------------





};
} /* namespace clustering */
#endif /* HDBSCAN_H_ */

