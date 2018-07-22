/*
 * hdbscan.h
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

#ifndef HDBSCAN_H_
#define HDBSCAN_H_

#ifdef __cplusplus
extern "C" {

#endif

#include <string.h>
#include <glib.h>
#include <float.h>
#include <stdio.h>
#include <gsl/gsl_statistics.h>
#include "cluster.h"
#include "constraint.h"
#include "distance.h"
#include "outlier_score.h"
#include "undirected_graph.h"
#include "gnulib/gl_array_oset.h"
#include "listlib/intlist.h"
#include "listlib/doublelist.h"

#define FILE_BUFFER_SIZE = 32678
#define HDBSCAN_SUCCESS 1
#define HDBSCAN_ERROR	0

#define CORE_DISTANCE_TYPE	0
#define INTRA_DISTANCE_TYPE	1

typedef GHashTable StringDoubleMap;
typedef GHashTable IntDistancesMap;

/**
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 * @author zjullion
 */

//string getWarningMessage();

#ifdef __cplusplus
namespace clustering {
#endif

typedef struct distance_values{
	double min_cr;				/// Minimum core distance in the cluster
	double max_cr;				/// Maximum core distance in the cluster
	double cr_confidence;		/// Cluster confidence based on core distances
	
	double max_dr;				/// Minimum actual distance in the cluster
	double min_dr;				/// Maximum actua distance in the cluster
	double dr_confidence;		/// Cluster confidence based on actual distances
} distance_values;

struct stats_values{
	double mean;
	double standardDev;
	double variance;
	double max;					/// 
	double kurtosis;
	double skewness;
};

typedef struct _clustering_stats{
	int32_t count;
	struct stats_values coreDistanceValues;
	struct stats_values intraDistanceValues;
} clustering_stats;

typedef struct _hierarchy_entry{
	double edgeWeight;
	int32_t* labels;
} hierarchy_entry;

typedef GHashTable LongHierarchyEntryMap;

struct hdbscan {
	distance distanceFunction;
	double* dataSet;
	UndirectedGraph* mst;
	ConstraintList* constraints;
	double* coreDistances;
	ClusterPtrList* clusters;
	outlier_score* outlierScores;
	int32_t* clusterLabels;
	LongHierarchyEntryMap* hierarchy;
	IntDoubleMap* clusterStabilities;
	boolean selfEdges;
	uint minPoints, minClusterSize, numPoints;	
	
#ifdef __cplusplus

public:

	hdbscan();
	hdbscan(uint minPts, uint datatype);
	~hdbscan();

	/**
	 *
	 */
	void run(void* dataset, uint rows, uint cols, boolean rowwise);
	void reRun(int32_t minPts);

	/**
	 * Calculates the core distances for each point in the data set, given some value for k.
	 * @param dataSet A vector<double>[] where index [i][j] indicates the jth attribute of data point i
	 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
	 * @param distanceFunction A DistanceCalculator to compute distances between points
	 */
	void calculateCoreDistances(void* dataSet, uint rows, uint cols);

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
	void computeHierarchyAndClusterTree(boolean compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters);

	/**
	 * Propagates constraint satisfaction, stability, and lowest child death level from each child
	 * cluster to each parent cluster in the tree.  This method must be called before calling
	 * findProminentClusters() or calculateOutlierScores().
	 * @param clusters A list of Clusters forming a cluster tree
	 * @return true if there are any clusters with infinite stability, false otherwise
	 */
	boolean propagateTree();

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
	void findProminentClusters(boolean infiniteStability);

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
	void calculateOutlierScores(double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability);
	
	void clean();
#endif


};

typedef struct hdbscan hdbscan;

/**
 * Initialise hdbcsan parameters
 * 
 */ 
hdbscan* hdbscan_init(hdbscan* sc, uint minPoints, uint datatype);

/**
 * Deallocate all memory including for the sc itself
 */
void hdbscan_destroy(hdbscan* sc);

/**
 * Clean up the memory for the attributes of hbdscan without deallocating
 * the memory of the hdbscan itself
 */
void hdbscan_clean(hdbscan* sc);

/**
 * Cluster the dataset 
 */
int hdbscan_run(hdbscan* sc, void* dataset, uint rows, uint cols, boolean rowwise);

/**
 * hdbscan_rerun
 * @param sc
 * @param minPts
 * 
 * In case you need to re-cluster with a differnt minPts without changing the dataset.
 * This function will do that by just recalculating the core distances from the existing
 * distances.
 * 
 */ 
int hdbscan_rerun(hdbscan* sc, int32_t minPts);

/**
 * Given min and max values of minPts, select the best minPts from min
 * to max inclusive.
 */
int32_t hdbscan_select_min_pts(int32_t min, int32_t max, void* dataset, int32_t datatype, IntIntListMap* selection, int32_t *val, int32_t *numClusters); 

/**
 * Create the minimum spanning tree
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
int32_t hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int32_t compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters);

/**
 * Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * @param clusters A list of Clusters forming a cluster tree
 * @return true if there are any clusters with infinite stability, false otherwise
 */
boolean hdbscan_propagate_tree(hdbscan* sc);

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
void hdbscan_find_prominent_clusters(hdbscan* sc, int32_t infiniteStability);

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
int hdbscsan_calculate_outlier_scores(hdbscan* sc, double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability);

/**
 * Given an array of labels, create a hash table where the keys are the labels and the values are the indices.
 **/
IntIntListMap* hdbscan_create_cluster_table(int32_t* labels, int32_t begin, int32_t end);

/**
 * Create a hash table that maps the different statistical values to their values.
 * The statistical value names are declared in the hdbscan.h header file. They are in the form
 * of NAME_{CR, DR}, where name is the name of the value and CR is for the value calculated using
 * the core distance values and DR is for 
 * 
 */ 
void hdbscan_calculate_stats(IntDistancesMap* distanceMap, clustering_stats* stats);

void hdbscan_calculate_stats_helper(double* cr, double* dr, clustering_stats* stats);

/**
 * Create a hash table for statistical values describing the clustering results
 */
int32_t hdbscan_analyse_stats(clustering_stats* stats); 

/**
 * Get the minimum and maximum core and intra-cluster distances
 */
IntDistancesMap* hdbscan_get_min_max_distances(hdbscan* sc, IntIntListMap* clusterTable);

/**
 * Sorts the clusters using the distances in the distanceMap.
 */
IntArrayList* hdbscan_sort_by_similarity(IntDistancesMap* distanceMap, IntArrayList *clusters, int32_t distanceType);

/**
 * Sorts clusters according to how long the cluster is
 */
IntArrayList* hdbscan_sort_by_length(IntIntListMap* clusterTable, IntArrayList *clusters); 

/**
 * Uses quick sort algorithm to sort clusters based on the data
 */ 
void hdbscan_quicksort(IntArrayList *clusters, DoubleArrayList *sortData, int32_t left, int32_t right);

/**
 * 
 * Destroy the hash tables
 */
void hdbscan_destroy_cluster_table(IntIntListMap* table);
void hdbscan_destroy_distance_map_table(IntDistancesMap* table);

/**
 * 
 */
hierarchy_entry* hdbscan_create_hierarchy_entry(); 

/**
 * Printing the hash tables
 * 
 */ 
void hdbscan_print_cluster_table(IntIntListMap* table);
void hdbscan_print_cluster_sizes(IntIntListMap* table);
void hdbscan_print_distance_map_table(IntDistancesMap* table);
void hdbscan_print_stats(clustering_stats* stats);
void hdbscan_print_hierarchies(LongHierarchyEntryMap* hierarchy, uint numPoints, char *filename);
#ifdef __cplusplus
};
}
#endif

#endif /* HDBSCAN_H_ */

