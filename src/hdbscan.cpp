/*
 * hdbscan.cpp
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


#ifdef __cplusplus
#include <limits>
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
void hdbscan::run(void* dataset, uint rows, uint cols, boolean rowwise, uint datatype){
	hdbscan_run(this, dataset, rows, cols, rowwise, datatype);
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

map_t createClusterMap(int32_t* labels, int32_t begin, int32_t end){

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


void printClusterMap(map_t& table){

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

void printClusterSizes(map_t& table){
	for(map_t::iterator it = table.begin(); it != table.end(); it++){

		int32_t label = it->first;
		vector<int>& clusterList = it->second;
		printf("%d : %ld\n", label, clusterList.size());
	}
}

void printDistanceMap(map<int32_t, distance_values>& distancesMap){

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

/**
 * Sorts the clusters using the distances in the distanceMap.
 */
void sortBySimilarity(map<int32_t, distance_values>& distanceMap, vector<int32_t>& clusters, int32_t distanceType)
{
	vector<double> distances;

	if(clusters.empty()){     /// If clusters had nothing in it, we will use the whole hash table

		for(map<int32_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it)
		{
			double conf;

			if(distanceType == CORE_DISTANCE_TYPE){
				conf = it->second.cr_confidence;
			} else{
				conf = it->second.dr_confidence;
			}

			clusters.push_back(it->first);
			distances.push_back(conf);
		}
	} else { /// else we just need to get the lengths from the hash table
		distances.resize(clusters.size());

#pragma omp parallel for
		for(size_t i = 0; i < clusters.size(); i++){
			int32_t key = clusters[i];
			map<int32_t, distance_values>::iterator it = distanceMap.find(key);
			double conf;

			if(distanceType == CORE_DISTANCE_TYPE){
				conf = it->second.cr_confidence;
			} else{
				conf = it->second.dr_confidence;
			}
			distances[i] = conf;
		}
	}

	// sort
	quickSort(clusters, distances, 0, clusters.size()-1);
}

/**
 * Sorts clusters according to how long the cluster is
 */
void sortByLength(map_t& clusterTable, vector<int32_t>& clusters)
{
	vector<double> lengths;

	if(clusters.empty()){     /// If clusters had nothing in it, we will use the whole hash table

		for(map_t::iterator it = clusterTable.begin(); it != clusterTable.end(); ++it)
		{
			clusters.push_back(it->first);
			lengths.push_back(it->second.size());
		}
	} else { /// else we just need to get the lengths from the hash table
		lengths.resize(clusters.size());
#pragma omp parallel for
		for(size_t i = 0; i < clusters.size(); i++){
			int32_t key = clusters[i];

			map_t::iterator it = clusterTable.find(key);
			lengths[i] = (double)it->second.size();
		}
	}
	// sort
	quickSort(clusters, lengths, 0, clusters.size()-1);
}

/**
 * Uses quick sort algorithm to sort clusters based on the data
 */
void quickSort(vector<int32_t>& clusters, vector<double>& sortData, int32_t left, int32_t right)
{
	DoubleArrayList* d_data = double_array_list_init();
	IntArrayList* c_data = int_array_list_init();

	for(size_t i = 0; i < clusters.size(); i++)
	{
		int_array_list_append(c_data, clusters[i]);
		double_array_list_append(d_data, sortData[i]);
	}

	hdbscan_quicksort(c_data, d_data, left, right);

	double* data_d = (double *)d_data->data;
	int32_t* data_c = (int32_t *)c_data->data;

	for(size_t i = 0; i < clusters.size(); i++)
	{
		clusters[i] = data_c[i];
		sortData[i] = data_d[i];
	}

	double_array_list_delete(d_data);
	int_array_list_delete(c_data);
}

};
#endif
