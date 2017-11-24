/*
 * hdbscan.cpp
 *
 *  Created on: 22 Sep 2017
 *      Author: junior
 */


#ifdef __cplusplus
#include <limits>
#include "config.h"
#include "hdbscan/hdbscan.hpp"
namespace clustering {


hdbscan::hdbscan(){
	hdbscan_init(this, 3, DATATYPE_DOUBLE);
}


hdbscan::hdbscan(uint minPts, uint datatype){
	hdbscan_init(this, minPts, datatype);
}

hdbscan::~hdbscan(){
	hdbscan_clean(this);
}

/**
 *
 */
void hdbscan::run(void* dataset, uint rows, uint cols, boolean rowwise){
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

void hdbscan::reRun(int32_t minPts){
	hdbscan_rerun(this, minPts);
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
void hdbscan::calculateOutlierScores(double* pointNoiseLevels, int* pointLastClusters, boolean infiniteStability){
	// TODO
}


void hdbscan::clean(){
	hdbscan_destroy(this);
}

map_t createClusterTable(int32_t* labels, int32_t begin, int32_t end){
	
	map_t clusterTable;
	
	for(int32_t i = begin; i < end; i++){
		int32_t label = labels[i];
		clusterTable[label].push_back(i);
	}
	
	return clusterTable;
	
}


map<int32_t, distance_values> getMinMaxDistances(hdbscan& scan, map_t& clusterTable){
	double* core = scan.distanceFunction.coreDistances;		
	map<int32_t, distance_values> pm;
	double zero = 0.00000000000;

	for(map_t::iterator it = clusterTable.begin(); it != clusterTable.end(); ++it){
		vector<int32_t> idxList = it->second;

		for(size_t i = 0; i < idxList.size(); i++){
			map<int32_t, distance_values>::iterator iter = pm.find(it->first);
			int32_t index = idxList[i];
			
			// min and max core distances
			if(iter == pm.end()){
				distance_values& dl = pm[it->first];
				dl.min_cr = core[index];
				dl.max_cr = core[index];
				dl.cr_confidence = 0.0;
				
				dl.min_dr = std::numeric_limits<double>::max();
				dl.max_dr = std::numeric_limits<double>::min();
				dl.dr_confidence = 0.0;
			} else{
				distance_values& dl = iter->second;
				// min core distance
				if(dl.min_cr > core[index] && (core[index] < zero || core[index] > zero)){
					dl.min_cr = core[index];
				}
								
				//max core distance
				if(dl.max_cr < core[index]){
					dl.max_cr = core[index];
				}				
			}
			
			// Calculating min and max distances
			for(size_t j = i+1; j < idxList.size(); j++){
				double d = distance_get(&scan.distanceFunction, index, idxList[j]);
				
				if(iter->second.min_dr > d && (d < zero || d > zero)){
					iter->second.min_dr = d;
				}

				// max distance
				if(iter->second.max_dr < d){
					iter->second.max_dr = d;
				}
			}
		}
	}
	
	return pm;
}

void calculateStats(map<int32_t, distance_values>& distanceMap, clustering_stats& stats){
	
	double cr[distanceMap.size()];
	double dr[distanceMap.size()];
	int c = 0;	
	
	for(map<int32_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it){	
		cr[c] = it->second.max_cr/it->second.min_cr;
		dr[c] = it->second.max_dr/it->second.min_dr;	
				
		c++;
	}
	stats.count = c;
	hdbscan_calculate_stats_helper(cr, dr, &stats);
	
	c = 0;
	for(map<int32_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it){	
		double rc = cr[c];
		double rd = dr[c];
		
		it->second.cr_confidence = ((stats.coreDistanceValues.max - rc) / stats.coreDistanceValues.max) * 100;
		it->second.dr_confidence = ((stats.intraDistanceValues.max - rd) / stats.intraDistanceValues.max) * 100;
					
		c++;
	}
}


int32_t analyseStats(clustering_stats& stats){
	
	return hdbscan_analyse_stats(&stats);
}


void printClusterTable(map_t& table){
	
	for(map_t::iterator it = table.begin(); it != table.end(); it++){
	
		int32_t label = it->first;
		vector<int>& clusterList = it->second;
		printf("%d -> [", label);
					
		for(size_t j = 0; j < clusterList.size(); j++){
			printf("%d ", clusterList[j]);
		}
		printf("]\n");
	}
}

void printDistanceMapTable(map<int32_t, distance_values>& distancesMap){

	printf("\n//////////////////////////////////////// Distances ///////////////////////////////////////////////\n");
	
	for(map<int32_t, distance_values>::iterator it = distancesMap.begin(); it != distancesMap.end(); it++){
		int32_t label = it->first;
		printf("%d -> {\n", label);
		distance_values* dv = &(it->second);
		
		printf("min_cr : %f, max_cr : %f, cr_confidence : %f\n", dv->min_cr, dv->max_cr, dv->cr_confidence);
		printf("min_dr : %f, max_dr : %f, dr_confidence : %f\n", dv->min_dr, dv->max_dr, dv->dr_confidence);
				
		printf("}\n\n");
	}
	printf("///////////////////////////////////////////////////////////////////////////////////////\n\n");
}

void printStats(clustering_stats& stats){
	hdbscan_print_stats(&stats);
}

};
#endif
