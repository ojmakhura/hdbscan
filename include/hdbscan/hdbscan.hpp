
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
#ifndef HDBSCAN_HPP_
#define HDBSCAN_HPP_

#ifdef __cplusplus


#include "hdbscan.h"

#include <map>
#include <vector>
#include <set>
#include <string>

using namespace std;

typedef map<int, vector<int>> map_t;
typedef map<int, vector<double>> map_d;
typedef set<int> set_t;


namespace clustering {
	/**
	 * 
	 **/
	map_t createClusterTable(int32_t* labels, int32_t begin, int32_t end); 	
	map<int32_t, distance_values> getMinMaxDistances(hdbscan& scan, map_t& clusterTable);
	void calculateStats(map<int32_t, distance_values>& distanceMap, clustering_stats& stats);	

	/**
	 * 
	 * 
	 */
	int32_t analyseStats(clustering_stats& stats); 
	
	void printClusterTable(map_t& table);
	
	void printClusterSizes(map_t& table);

	void printDistanceMapTable(map<int32_t, distance_values>& distancesMap);

	void printStats(clustering_stats& stats);
};
#endif
#endif /* HDBSCAN_HPP_ */
