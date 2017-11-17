/*
 * hdbscan.cpp
 *
 *  Created on: 22 Sep 2017
 *      Author: junior
 */


#ifdef __cplusplus
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


map_d getMinMaxDistances(hdbscan& scan, map_t& clusterTable){
	//map<int32_t, vector<double>> disMap;
	double* core = scan.distanceFunction.coreDistances;	
	//double zero = 0.00000000000;
	
	map_d pm;
	double zero = 0.00000000000;

	for(map_t::iterator it = clusterTable.begin(); it != clusterTable.end(); ++it){
		vector<int> idc = it->second;

		for(size_t i = 0; i < idc.size(); i++){

			// min and max core distances
			if(pm[it->first].size() == 0){
				pm[it->first].push_back(core[idc[i]]);
				pm[it->first].push_back(core[idc[i]]);
			} else{
				// min core distance
				if(pm[it->first][0] > core[idc[i]] && (core[idc[i]] < zero || core[idc[i]] > zero)){
					pm[it->first][0] = core[idc[i]];
				}

				//max core distance
				if(pm[it->first][1] < core[idc[i]]){
					pm[it->first][1] = core[idc[i]];
				}
			}

			// Calculating min and max distances
			for(size_t j = i+1; j < idc.size(); j++){
				double d = distance_get(&scan.distanceFunction, i, j);

				if(pm[it->first].size() == 2){
					pm[it->first].push_back(d);
					pm[it->first].push_back(d);
				} else{
					// min distance
					if(pm[it->first][2] > d && (d < zero || d > zero)){
						pm[it->first][2] = d;
					}

					// max distance
					if(pm[it->first][3] < d){
						pm[it->first][3] = d;
					}
				}
			}
		}
	}
	
	return pm;
}

map<string, double> calculateStats(map_d& distanceMap){
	map<string, double> statsMap;
	
	double cr[distanceMap.size()];
	double dr[distanceMap.size()];
	int c = 0;
	
	for(map_d::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it){			
		cr[c] = (it->second)[1]/(it->second)[0];
		dr[c] = (it->second)[3]/(it->second)[2];
		c++;
	}
	
	// Calculating core distance statistics
	statsMap[get_mean_cr()] = gsl_stats_mean(cr, 1, c);
	statsMap[get_sd_cr()] = gsl_stats_sd(cr, 1, c);	
	statsMap[get_variance_cr()] = gsl_stats_variance(cr, 1, c);
	statsMap[get_max_cr()] = gsl_stats_max(cr, 1, c);
	statsMap[get_kurtosis_cr()] = gsl_stats_kurtosis(cr, 1, c);
	statsMap[get_skew_cr()] = gsl_stats_skew(cr, 1, c);
	
	// calculating intra distance statistics
	statsMap[get_mean_dr()] = gsl_stats_mean(dr, 1, c);
	statsMap[get_sd_dr()] = gsl_stats_sd(dr, 1, c);
	statsMap[get_variance_dr()] = gsl_stats_variance(dr, 1, c);
	statsMap[get_max_dr()] = gsl_stats_max(dr, 1, c);	
	statsMap[get_kurtosis_dr()] = gsl_stats_kurtosis(dr, 1, c);
	statsMap[get_skew_dr()] = gsl_stats_skew(dr, 1, c);
	
	statsMap[get_count()] = c;
	
	return statsMap;
}


int32_t analyseStats(map<string, double>& stats){
	int32_t validity = -1;
	double skew_cr = stats[get_skew_cr()];	
	double skew_dr = stats[get_skew_dr()];	
	double kurtosis_cr = stats[get_kurtosis_cr()];	
	double kurtosis_dr = stats[get_kurtosis_dr()];	
	
	if((skew_dr > 0.0 ) && (kurtosis_dr > 0.0 )){
		validity = 2;
	} else if(skew_dr < 0.0 && kurtosis_dr > 0.0){
		validity = 1;
	} else if(skew_dr > 0.0 && kurtosis_dr < 0.0){
		validity = 0;
	} else{
		validity = -1;
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

void printDistanceMapTable(map_d& distancesMap){

	printf("\n///////////////////////////////////////////////////////////////////////////////////////\n");
	
	for(map_d::iterator it = distancesMap.begin(); it != distancesMap.end(); it++){
		int32_t label = it->first;
		printf("%d -> [", label);
		vector<double>& list = it->second;
		
		for(size_t i = 0; i < list.size(); i++){
			printf("%f ", list[i]);
		}
		
		printf("]\n");
	}
	printf("///////////////////////////////////////////////////////////////////////////////////////\n\n");
}

void printStatsMap(map<string, double>& table){
	
	printf("///////////////////////////////////////////////////////////////////////////////////////\n");
	
	for(map<string, double>::iterator it = table.begin(); it != table.end(); it++){
		string label = it->first;
		printf("%s : %f\n", label.c_str(), it->second);
	}
	printf("///////////////////////////////////////////////////////////////////////////////////////\n\n");
}

};
#endif
