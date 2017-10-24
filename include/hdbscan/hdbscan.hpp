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
	map_t createClusterTable(int32_t* labels, int32_t size); 	
	map_d getMinMaxDistances(hdbscan& scan, map_t& clusterTable);
	map<string, double> calculateStats(map_d& distanceMap);	

	/**
	 * 
	 * 
	 */
	int32_t analyseStats(map<string, double>& stats); 
	
	void printClusterTable(map_t& table);

	void printDistanceMapTable(map_d& distancesMap);

	void printStatsMap(map<string, double>& table);
};
#endif
#endif /* HDBSCAN_HPP_ */
