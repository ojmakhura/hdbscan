/*
 * hdbscan.h
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef HDBSCAN_H_
#define HDBSCAN_H_

#ifdef __cplusplus
extern "C" {

#endif

#include <string.h>
#include <glib.h>
#include <float.h>
#include <gsl/gsl_statistics.h>
#include "config.h"
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

typedef GHashTable StringDoubleMap;


/**
 * Core distance funtions
 */  
inline const char* get_mean_cr(){
	return "mean_cr";
}

inline const char* get_sd_cr(){
	return "sd_cr";
}

inline const char* get_variance_cr(){
	return "variance_cr";
}

inline const char* get_max_cr(){
	return "max_cr";
}

inline const char* get_kurtosis_cr(){
	return "kurtosis_cr";
}

inline const char* get_skew_cr(){
	return "skew_cr";
}
/**
 * intra cluster distance funtions
 */  
inline const char* get_mean_dr(){
	return "mean_dr";
}

inline const char* get_sd_dr(){
	return "sd_dr";
}

inline const char* get_variance_dr(){
	return "variance_dr";
}

inline const char* get_max_dr(){
	return "max_dr";
}

inline const char* get_kurtosis_dr(){
	return "kurtosis_dr";
}

inline const char* get_skew_dr(){
	return "skew_dr";
}

inline const char* get_count(){
	return "count";
}
/*
char MEAN_CR[] = "mean_cr";
char SD_CR[] = "sd_cr";
char VARIANCE_CR[] = "variance_cr";
char MAX_CR[] = "max_cr";
char KURTOSIS_CR[] = "kurtosis_cr";
char SKEW_CR[] = "skew_cr";
char MEAN_DR[] = "mean_dr";
char SD_DR[] = "sd_dr";
char VARIANCE_DR[] = "variance_dr";
char MAX_DR[] = "max_dr";
char KURTOSIS_DR[] = "kurtosis_dr";
char SKEW_DR[] = "skew_dr";
char COUNT[] = "count";
*/
/**
 * Implementation of the HDBSCAN* algorithm, which is broken into several methods.
 * @author zjullion
 */

//string getWarningMessage();

#ifdef __cplusplus
namespace clustering {
#endif

struct hdbscan {
	distance distanceFunction;
	double* dataSet;
	UndirectedGraph* mst;
	ConstraintList* constraints;
	double* coreDistances;
	ClusterPtrList* clusters;
	outlier_score* outlierScores;
	int32_t* clusterLabels;
	LongIntListMap* hierarchy;
	IntDoubleMap* clusterStabilities;
	//IntIntListMap* clusterTable;
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
 *
 */
int hdbscan_run(hdbscan* sc, void* dataset, uint rows, uint cols, boolean rowwise);

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
StringDoubleMap* hdbscan_calculate_stats(IntDoubleListMap* distanceMap);

/**
 * 
 * 
 */
int32_t hdbscan_analyse_stats(StringDoubleMap* stats); 

/**
 *
 */
IntDoubleListMap* hdbscan_get_min_max_distances(hdbscan* sc, IntIntListMap* clusterTable);

/**
 * 
 * Destroy the hash tables
 */
void hdbscan_destroy_cluster_table(IntIntListMap* table);
void hdbscan_destroy_distance_map_table(IntDoubleListMap* table);
void hdbscan_destroy_stats_map(StringDoubleMap* statsMap);

/**
 * Printing the hash tables
 * 
 */ 
void hdbscan_print_cluster_table(IntIntListMap* table);
void hdbscan_print_distance_map_table(IntDoubleListMap* table);
void hdbscan_print_stats_map(StringDoubleMap* statsMap);
#ifdef __cplusplus
};
}
#endif

#endif /* HDBSCAN_H_ */

