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

/**
 * @file hdbscan.cpp
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the C++ versions of the C functions. Most of the functions
 * merely call the C functions. Only those files who use C++ native code and structures
 * are fully implemented here.
 * 
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifdef __cplusplus
#include <limits>
#include "hdbscan/hdbscan.hpp"
namespace clustering {


hdbscan::hdbscan(){
	hdbscan_init(this, 3);
}

hdbscan::hdbscan(index_t minPts){
	hdbscan_init(this, minPts);
}

hdbscan::~hdbscan(){
	hdbscan_clean(this);
}

void hdbscan::run(void* dataset, index_t rows, index_t cols, boolean rowwise, index_t datatype){
	hdbscan_run(this, dataset, rows, cols, rowwise, datatype);
}

void hdbscan::constructMST(){
	hdbscan_construct_mst(this);
}

void hdbscan::reRun(index_t minPts){
	hdbscan_rerun(this, minPts);
}

void hdbscan::computeHierarchyAndClusterTree(boolean compactHierarchy, distance_t* pointNoiseLevels, label_t* pointLastClusters){
	hdbscan_compute_hierarchy_and_cluster_tree(this, compactHierarchy, pointNoiseLevels, pointLastClusters);
}

boolean hdbscan::propagateTree(){
	return hdbscan_propagate_tree(this);
}

void hdbscan::findProminentClusters(boolean infiniteStability){
	hdbscan_find_prominent_clusters(this, infiniteStability);
}

void hdbscan::calculateOutlierScores(distance_t* pointNoiseLevels, label_t* pointLastClusters, boolean infiniteStability){
	// TODO
}

void hdbscan::clean(){
	hdbscan_destroy(this);
}

map_t createClusterMap(label_t* labels, index_t begin, index_t end){

	map_t clusterTable;

	for(index_t i = begin; i < end; i++){
		label_t label = labels[i];
		clusterTable[label].push_back(i);
	}

	return clusterTable;
}

map<label_t, distance_values> getMinMaxDistances(hdbscan& scan, map_t& clusterTable){
	distance_t* core = scan.distanceFunction.coreDistances;
	map<label_t, distance_values> pm;
	distance_t zero = 0.0000000000000000000;

	for(map_t::iterator it = clusterTable.begin(); it != clusterTable.end(); ++it){
		vector<index_t> idxList = it->second;
		for(size_t i = 0; i < idxList.size(); i++){
			map<label_t, distance_values>::iterator iter = pm.find(it->first);
			index_t index = idxList[i];

			// min and max core distances
			if(iter == pm.end()){
				distance_values& dl = pm[it->first];
				dl.min_cr = core[index];
				dl.max_cr = core[index];
				dl.cr_confidence = 0.0;

				dl.min_dr = std::numeric_limits<distance_t>::max();
				dl.max_dr = std::numeric_limits<distance_t>::min();
				dl.dr_confidence = 0.0;
				iter = pm.find(it->first);
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
			for(index_t j = (index_t)i+1; j < idxList.size(); j++){
				distance_t d = distance_get(&scan.distanceFunction, index, idxList[j]);

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

void calculateStats(map<label_t, distance_values>& distanceMap, clustering_stats& stats){

	distance_t cr[distanceMap.size()];
	distance_t dr[distanceMap.size()];
	int c = 0;

	for(map<label_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it){
		cr[c] = it->second.max_cr/it->second.min_cr;
		dr[c] = it->second.max_dr/it->second.min_dr;

		c++;
	}
	stats.count = c;
	hdbscan_calculate_stats_helper(cr, dr, &stats);

	c = 0;
	for(map<label_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it){
		distance_t rc = cr[c];
		distance_t rd = dr[c];

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

		label_t label = it->first;
		vector<index_t>& clusterList = it->second;
		printf("%d -> [", label);

		for(index_t j = 0; j < clusterList.size(); j++){
			printf("%d ", clusterList[j]);
		}
		printf("]\n");
	}
}

void printClusterSizes(map_t& table){
	for(map_t::iterator it = table.begin(); it != table.end(); it++){

		label_t label = it->first;
		vector<index_t>& clusterList = it->second;
		printf("%d : %ld\n", label, clusterList.size());
	}
}

void printDistanceMap(map<label_t, distance_values>& distancesMap){

	printf("\n//////////////////////////////////////// Distances ///////////////////////////////////////////////\n");

	for(map<label_t, distance_values>::iterator it = distancesMap.begin(); it != distancesMap.end(); it++){
		label_t label = it->first;
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
void sortBySimilarity(map<label_t, distance_values>& distanceMap, vector<label_t>& clusters, int32_t distanceType)
{
	vector<distance_t> distances;

	if(clusters.empty()){     /// If clusters had nothing in it, we will use the whole hash table

		for(map<label_t, distance_values>::iterator it = distanceMap.begin(); it != distanceMap.end(); ++it)
		{
			distance_t conf;

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
#ifdef _OPENMP
#pragma omp parallel for
#endif
		for(size_t i = 0; i < clusters.size(); i++){
			label_t key = clusters[i];
			map<label_t, distance_values>::iterator it = distanceMap.find(key);
			distance_t conf;

			if(distanceType == CORE_DISTANCE_TYPE){
				conf = it->second.cr_confidence;
			} else{
				conf = it->second.dr_confidence;
			}
			distances[i] = conf;
		}
	}

	// sort
	quickSort(clusters, distances, 0, (index_t)clusters.size()-1);
}

/**
 * Sorts clusters according to how long the cluster is
 */
void sortByLength(map_t& clusterTable, vector<label_t>& clusters)
{
	vector<distance_t> lengths;

	if(clusters.empty()){     /// If clusters had nothing in it, we will use the whole hash table

		for(map_t::iterator it = clusterTable.begin(); it != clusterTable.end(); ++it)
		{
			clusters.push_back(it->first);
			lengths.push_back((index_t)(it->second.size()));
		}
	} else { /// else we just need to get the lengths from the hash table
		lengths.resize(clusters.size());

		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for(size_t i = 0; i < clusters.size(); i++){
			label_t key = clusters[i];

			map_t::iterator it = clusterTable.find(key);
			lengths[i] = (distance_t)it->second.size();
		}
	}
	// sort
	quickSort(clusters, lengths, 0, (index_t)clusters.size()-1);
}

/**
 * Uses quick sort algorithm to sort clusters based on the data
 */
void quickSort(vector<label_t>& clusters, vector<distance_t>& sortData, index_t left, index_t right)
{
	ArrayList* d_data = array_list_init(clusters.size(), sizeof(distance_t), NULL);
	if(sizeof(distance_t) == sizeof(double)) {
		d_data->compare = double_compare;
	} else {
		d_data->compare = float_compare;
	}

	ArrayList* c_data = array_list_init(clusters.size(), sizeof(label_t), NULL);
	
	if(sizeof(label_t) == sizeof(int)) {
		c_data->compare = int_compare;
	} else if(sizeof(label_t) == sizeof(long)) {
		c_data->compare = long_compare;
	} else {
		c_data->compare = short_compare;
	}

	for(size_t i = 0; i < clusters.size(); i++)
	{
		label_t l = clusters[i];
		array_list_append(c_data, &l);
		distance_t t = sortData[i];
		array_list_append(d_data, &t);
	}

	hdbscan_quicksort(c_data, d_data, left, right);

	distance_t* data_d = (distance_t *)d_data->data;
	label_t* data_c = (label_t *)c_data->data;

	for(size_t i = 0; i < clusters.size(); i++)
	{
		clusters[i] = data_c[i];
		sortData[i] = data_d[i];
	}

	array_list_delete(d_data);
	array_list_delete(c_data);
}

};
#endif
