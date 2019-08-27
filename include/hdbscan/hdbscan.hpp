
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

/** @file hdbscan.hpp */
#ifndef HDBSCAN_HPP_
#define HDBSCAN_HPP_

#ifdef __cplusplus

#include "hdbscan.h"

#include <map>
#include <vector>
#include <set>
#include <string>

using namespace std;

typedef map<label_t, vector<index_t>> map_t;
typedef map<label_t, vector<distance_t>> map_d;
typedef set<index_t> set_i;


namespace clustering {
	
	/**
	 * @brief Create a Cluster Map object
	 * 
	 * @param labels 
	 * @param begin 
	 * @param end 
	 * @return map_t 
	 */
	map_t createClusterMap(label_t* labels, index_t begin, index_t end);

	/**
	 * @brief Get the Min Max Distances object
	 * 
	 * @param scan 
	 * @param clusterTable 
	 * @return map<int32_t, distance_values> 
	 */
	map<label_t, distance_values> getMinMaxDistances(hdbscan& scan, map_t& clusterTable);

	/**
	 * @brief 
	 * 
	 * @param distanceMap 
	 * @param stats 
	 */
	void calculateStats(map<label_t, distance_values>& distanceMap, clustering_stats& stats);

	/**
	 * @brief 
	 * 
	 * @param stats 
	 * @return int32_t 
	 */
	int32_t analyseStats(clustering_stats& stats);

	/**
	 * @brief 
	 * 
	 * @param table 
	 */
	void printClusterMap(map_t& table);

	/**
	 * @brief 
	 * 
	 * @param table 
	 */
	void printClusterSizes(map_t& table);

	/**
	 * @brief 
	 * 
	 * @param distancesMap 
	 */
	void printDistanceMap(map<label_t, distance_values>& distancesMap);

	/**
	 * @brief 
	 * 
	 * @param stats 
	 */
	void printStats(clustering_stats& stats);

	/**
	 * @brief Sorts the clusters using the distances in the distanceMap.
	 * 
	 * @param distanceMap 
	 * @param clusters 
	 * @param distanceType 
	 */
	void sortBySimilarity(map<label_t, distance_values>& distanceMap, vector<label_t>& clusters, int32_t distanceType);
	
	/**
	 * @brief Sorts clusters according to how long the cluster is
	 * 
	 * @param clusterTable 
	 * @param clusters 
	 */
	void sortByLength(map_t& clusterTable, vector<label_t>& clusters);

	/**
	 * @brief 
	 * 
	 * @param clusters 
	 * @param sortData 
	 * @param left 
	 * @param right 
	 */
	void quickSort(vector<label_t>& clusters, vector<distance_t>& sortData, index_t left, index_t right);
};
#endif
#endif /* HDBSCAN_HPP_ */
