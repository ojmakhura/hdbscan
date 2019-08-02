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

/** @file hdbscan.h */

#ifndef HDBSCAN_H_
#define HDBSCAN_H_

#ifdef __cplusplus
extern "C" {

#endif
//#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
//#include <glib.h>
#include <float.h>
#include <stdio.h>
#include "cluster.h"
#include "constraint.h"
#include "distance.h"
#include "outlier_score.h"
#include "undirected_graph.h"
//#include "gnulib/gl_array_oset.h"
#include "listlib/intlist.h"
#include "listlib/doublelist.h"
#include "listlib/hashtable.h"

#define FILE_BUFFER_SIZE = 32678
#define HDBSCAN_SUCCESS 1
#define HDBSCAN_ERROR	0

#define CORE_DISTANCE_TYPE	0
#define INTRA_DISTANCE_TYPE	1

typedef hashtable StringDoubleMap;
typedef hashtable IntDistancesMap;
typedef hashtable IntIntListMap;			// Hash table with keys as int and values as a list of integers
typedef hashtable LongIntListMap;
typedef hashtable LongIntPointerMap;
typedef hashtable IntDoubleMap;
typedef hashtable IntDoubleListMap;
typedef hashtable IntClusterListMap;

/**\typedef LongHierarchyEntryMap*/
//typedef GHashTable LongHierarchyEntryMap;

/**
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 * @author ojmakhura
 */

//string getWarningMessage();

#ifdef __cplusplus
namespace clustering {
#endif

/**
 * \struct distance_values
 * 
 * @brief The structure for keeping the intra and core
 * distance min, max and confidence values.
 */
typedef struct distance_values{
	double min_cr;				/// Minimum core distance in the cluster
	double max_cr;				/// Maximum core distance in the cluster
	double cr_confidence;		/// Cluster confidence based on core distances

	double max_dr;				/// Minimum actual distance in the cluster
	double min_dr;				/// Maximum actua distance in the cluster
	double dr_confidence;		/// Cluster confidence based on actual distances
} distance_values; /** \typedef distance_values */

/**
 * \struct stats_values
 * @brief The statistical values
 */
typedef struct stats_values{
	double mean;
	double standardDev;
	double variance;
	double max;					///
	double kurtosis;
	double skewness;
} stats_values; /** \typedef stats_values */

/**
 * \struct _clustering_stats
 * @brief Clustering statistics for the intra and core distances
 * 
 */
typedef struct _clustering_stats{
	int32_t count;
	stats_values coreDistanceValues;
	stats_values intraDistanceValues;
} clustering_stats; /** \typedef clustering_stats */

/**
 * \struct _hierarchy_entry
 * @brief An entry into the hierarchy
 * 
 */
typedef struct _hierarchy_entry{
	double edgeWeight;
	int32_t* labels;
} hierarchy_entry; /** \typedef hierarchy_entry */

/**
 * \struct hdbscan
 * @brief Main HDBSCAN object
 * 
 */
struct hdbscan {
	distance distanceFunction;				/// The distance function calculator object
	double* dataSet;						/// The dataset
	UndirectedGraph* mst;					/// The dendogram graph
	ArrayList* constraints;					/// Constraints
	double* coreDistances;					/// Core distances
	ArrayList* clusters;
	outlier_score* outlierScores;
	int32_t* clusterLabels;
	hashtable* hierarchy;
	IntDoubleMap* clusterStabilities;
	boolean selfEdges;
	uint minPoints, minClusterSize, numPoints;

#ifdef __cplusplus

public:
	/**
	 * @brief Construct a new hdbscan object
	 * 
	 */
	hdbscan();

	/**
	 * @brief Construct a new hdbscan object
	 * 
	 * @param minPts 
	 */
	hdbscan(uint minPts);

	/**
	 * @brief Destroy the hdbscan object
	 * 
	 */
	~hdbscan();

	/**
	 * @brief Find the clusters in the data for the first time. This function
	 * must be called before calling reRun
	 * 
	 * @param dataset 
	 * @param rows 
	 * @param cols 
	 * @param rowwise 
	 * @param datatype 
	 */
	void run(void* dataset, uint rows, uint cols, boolean rowwise, uint datatype);

	/**
	 * @brief Re-runs HDBSCAN without re-calculating the distances. It MUST be run after run()
	 * 
	 * @param minPts 
	 */
	void reRun(int32_t minPts);

	/**
	 * @brief Calculates the core distances for each point in the data set.
	 * 
	 * @param dataSet 
	 * @param rows 
	 * @param cols 
	 */
	void calculateCoreDistances(void* dataSet, uint rows, uint cols);

	
	/**
	 * @brief Constructs the minimum spanning tree of mutual reachability distances for the data set, given
	 * the core distances for each point.
	 * 
	 */
	void constructMST();

	/**
	 * @brief Computes the hierarchy and cluster tree from the minimum spanning tree,
	 * and returns the cluster tree.  
	 * 
	 * Additionally, the level at which each point becomes noise is
	 * computed.  Note that the minimum spanning tree may also have self edges (meaning it is not
	 * a true MST).
	 * 
	 * @param compactHierarchy 
	 * @param pointNoiseLevels A double array to be filled with the levels at which each point becomes noise
	 * @param pointLastClusters An int array to be filled with the last label each point had before becoming noise
	 */
	void computeHierarchyAndClusterTree(boolean compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters);

	/**
	 * @brief Propagates constraint satisfaction, stability, and lowest child death level from each child
	 * cluster to each parent cluster in the tree.  This method must be called before calling
	 * 
	 * @return boolean true if there are any clusters with infinite stability, false otherwise
	 */
	boolean propagateTree();

	/**
	 * @brief Produces a flat clustering result using constraint satisfaction and cluster stability, and
	 * returns an array of labels.  propagateTree() must be called before calling this method.
	 * 
	 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
	 */
	void findProminentClusters(boolean infiniteStability);

	/**
	 * @brief Produces the outlier score for each point in the data set, and returns a sorted list of outlier
	 * scores.  
	 * 
	 * propagateTree() must be called before calling this method.
	 * 
	 * @param pointNoiseLevels A double array with the levels at which each point became noise
	 * @param pointLastClusters An int array with the last label each point had before becoming noise
	 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
	 */
	void calculateOutlierScores(double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability);

	/**
	 * @brief C++ version of hdbscan_clean
	 * 
	 */
	void clean();
#endif
};

/**\typedef hdbscan */
typedef struct hdbscan hdbscan;

/**
 * @brief Initialise hdbcsan parameters
 * 
 * @param sc 
 * @param minPoints HDBSCAN's minPts parameter
 * @return hdbscan* 
 */
hdbscan* hdbscan_init(hdbscan* sc, uint minPoints);

/**
 * @brief Deallocate all memory including for the sc itself
 * 
 * @param sc 
 */
void hdbscan_destroy(hdbscan* sc);

/**
 * @brief Clean up the memory for the attributes of hbdscan without deallocating
 * the memory of the hdbscan itself
 * 
 * @param sc 
 */
void hdbscan_clean(hdbscan* sc);

/**
 * @brief Run HDBSCAN cluster detection on the dataset.
 * 
 * @param sc 
 * @param dataset 
 * @param rows 
 * @param cols 
 * @param rowwise 
 * @param datatype 
 * @return int 
 */
int hdbscan_run(hdbscan* sc, void* dataset, uint rows, uint cols, boolean rowwise, uint datatype);

/**
 * @brief In case you need to re-cluster with a differnt minPts without changing the dataset.
 * This function will do that by just recalculating the core distances from the existing
 * distances.
 * 
 * @param sc 
 * @param minPts 
 * @return int 
 */
int hdbscan_rerun(hdbscan* sc, int32_t minPts);

/**
 * @brief Given min and max values of minPts, select the best minPts from min
 * to max inclusive.
 * 
 * @param min 
 * @param max 
 * @param dataset 
 * @param datatype 
 * @param selection 
 * @param val 
 * @param numClusters 
 * @return int32_t 
 */
int32_t hdbscan_select_min_pts(int32_t min, int32_t max, void* dataset, int32_t datatype, hashtable* selection, int32_t *val, int32_t *numClusters);

/**
 * @brief Create the minimum spanning tree
 * 
 * @param sc 
 * @return int 
 */
int hdbscan_construct_mst(hdbscan* sc);

/**
 * @brief Computes the hierarchy and cluster tree from the minimum spanning tree, writing both to file,
 * and returns the cluster tree.  Additionally, the level at which each point becomes noise is
 * computed.  Note that the minimum spanning tree may also have self edges (meaning it is not
 * a true MST).
 * 
 * @param sc 
 * @param compactHierarchy 
 * @param pointNoiseLevels A double array to be filled with the levels at which each point becomes noise
 * @param pointLastClusters An int array to be filled with the last label each point had before becoming noise
 * @return int32_t 
 */
int32_t hdbscan_compute_hierarchy_and_cluster_tree(hdbscan* sc, int32_t compactHierarchy, double* pointNoiseLevels, int32_t* pointLastClusters);

/**
 * @brief Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * 
 * @param sc 
 * @return boolean true if there are any clusters with infinite stability, false otherwise
 */
boolean hdbscan_propagate_tree(hdbscan* sc);

/**
 * @brief Produces a flat clustering result using constraint satisfaction and cluster stability, and
 * returns an array of labels.  hdbscan_propagate_tree() must be called before calling this method.
 * 
 * @param sc 
 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
 */
void hdbscan_find_prominent_clusters(hdbscan* sc, int32_t infiniteStability);

/**
 * @brief Produces the outlier score for each point in the data set, and returns a sorted list of outlier
 * scores.  hdbscan_propagate_tree() must be called before calling this method.
 * 
 * @param sc 
 * @param pointNoiseLevels A vector<double> with the levels at which each point became noise
 * @param pointLastClusters An vector<int> with the last label each point had before becoming noise
 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
 * @return int 
 */
int hdbscsan_calculate_outlier_scores(hdbscan* sc, double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability);

/**
 * @brief Given an array of labels, create a hash table where the keys are the labels and the values are the indices.
 * 
 * @param labels 
 * @param begin 
 * @param end 
 * @return IntIntListMap* 
 */
hashtable* hdbscan_create_cluster_map(int32_t* labels, int32_t begin, int32_t end);

/**
 * @brief Create a hash table that maps the different statistical values to their values.
 * The statistical value names are declared in the hdbscan.h header file. They are in the form
 * of NAME_{CR, DR}, where name is the name of the value and CR is for the value calculated using
 * the core distance values and DR is for
 * 
 * @param distanceMap 
 * @param stats 
 */
void hdbscan_calculate_stats(hashtable* distanceMap, clustering_stats* stats);

/**
 * @brief A helper function for calculating statical values
 * 
 * @param cr 
 * @param dr 
 * @param stats 
 */
void hdbscan_calculate_stats_helper(double* cr, double* dr, clustering_stats* stats);

/**
 * @brief Create a hash table for statistical values describing the clustering results
 * 
 * @param stats 
 * @return int32_t 
 */
int32_t hdbscan_analyse_stats(clustering_stats* stats);

/**
 * @brief Get the minimum and maximum core and intra-cluster distances
 * 
 * @param sc 
 * @param clusterTable 
 * @return IntDistancesMap* 
 */
hashtable* hdbscan_get_min_max_distances(hdbscan* sc, hashtable* clusterTable);

/**
 * @brief Sorts the clusters using the distances in the distanceMap.
 * 
 * @param distanceMap 
 * @param clusters 
 * @param distanceType 
 * @return IntArrayList* 
 */
IntArrayList* hdbscan_sort_by_similarity(hashtable* distanceMap, IntArrayList *clusters, int32_t distanceType);

/**
 * @brief Sorts clusters according to how the size of the clusters
 * 
 * @param clusterTable 
 * @param clusters 
 * @return IntArrayList* 
 */
IntArrayList* hdbscan_sort_by_length(hashtable* clusterTable, IntArrayList *clusters);

/**
 * @brief Uses quick sort algorithm to sort clusters based on the data
 * 
 * @param clusters 
 * @param sortData 
 * @param left 
 * @param right 
 */
void hdbscan_quicksort(IntArrayList *clusters, DoubleArrayList *sortData, int32_t left, int32_t right);

/**
 * @brief Deallocate the memory used for the cluster map
 * 
 * @param table 
 */
void hdbscan_destroy_cluster_map(hashtable* table);

/**
 * @brief Deallocate the memory used for the distance map.
 * 
 * @param table 
 */
void hdbscan_destroy_distance_map(hashtable* table);

/**
 * @brief 
 * 
 * @return hierarchy_entry* 
 */
hierarchy_entry* hdbscan_create_hierarchy_entry();

//!
//! @brief Print the cluster map
//! 
//! @param table 
void hdbscan_print_cluster_map(hashtable* table);

/**
 * @brief Print the cluster sizes
 * 
 * @param table 
 */
void hdbscan_print_cluster_sizes(hashtable* table);

/**
 * @brief Printing the distance map to the console
 * 
 * @param table 
 */
void hdbscan_print_distance_map(hashtable* table);

/**
 * @brief Print the stastical values
 * 
 * @param stats 
 */
void hdbscan_print_stats(clustering_stats* stats);

/**
 * @brief Print the cluster hierarchy
 * 
 * @param hierarchy 
 * @param numPoints 
 * @param filename 
 */
void hdbscan_print_hierarchies(hashtable* hierarchy, uint numPoints, char *filename);

void hdbscan_destroy_hierarchical_entry(hierarchy_entry* entry);
#ifdef __cplusplus
};
}
#endif

#endif /* HDBSCAN_H_ */

