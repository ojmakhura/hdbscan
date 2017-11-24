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

	void printDistanceMapTable(map<int32_t, distance_values>& distancesMap);

	void printStats(clustering_stats& stats);
};
#endif
#endif /* HDBSCAN_HPP_ */
