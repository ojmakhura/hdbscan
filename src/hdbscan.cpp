/*
 * hdbscan.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "hdbscan/hdbscan.hpp"
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <list>
#include <set>
#include<string>

using namespace clustering;
using namespace clustering::distance;
namespace clustering {


string getWarningMessage() {
	string message =
			"----------------------------------------------- WARNING -----------------------------------------------\n";

	message +=
			"(infinite) for some data objects, either due to replicates in the data (not a set) or due to numerical\n";
	message +=
			"roundings. This does not affect the construction of the density-based clustering hierarchy, but\n";
	message +=
			"it affects the computation of cluster stability by means of relative excess of mass. For this reason,\n";
	message +=
			"the post-processing routine to extract a flat partition containing the most stable clusters may\n";
	message +=
			"produce unexpected results. It may be advisable to increase the value of MinPts and/or M_clSize.\n";
	message +=
			"-------------------------------------------------------------------------------------------------------";

	return message;
}

template <class T1>
hdbscan<T1>::hdbscan(){
	this->minPoints = 0;
	distanceFunction.setCalculator(_EUCLIDEAN);
	distanceFunction.setCalculator(_EUCLIDEAN);
	selfEdges = true;
	//mst = NULL;
	numPoints = 0;
}

template <class T1>
hdbscan<T1>::hdbscan(calculator cal, uint minPoints) {
	this->minPoints = minPoints;
	distanceFunction.setCalculator(cal);
	selfEdges = true;
	numPoints = 0;
}

template <class T1>
hdbscan<T1>::~hdbscan() {

	this->clean();

}

/**
 * Calculates the core distances for each point in the data set, given some value for k.
 * @param dataSet A vector<vector<double> > where index [i][j] indicates the jth attribute of data point i
 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
 * @param distanceFunction A DistanceCalculator to compute distances between points
 * @return An array of core distances
 */
template <class T1>
void hdbscan<T1>::calculateCoreDistances(T1* dataSet, int rows, int cols) {

	uint size = rows;

	int numNeighbors = minPoints - 1;

	if (minPoints == 1 && size < minPoints) {
		return;
	}

	distanceFunction.computeDistance(dataSet, rows, cols, numNeighbors);

}

template <class T1>
void hdbscan<T1>::constructMST() {

	//double* distances = distanceFunction.getDistance();
	double* coreDistances = distanceFunction.getCoreDistances();

	int selfEdgeCapacity = 0;
	uint size = numPoints;
	if (selfEdges){
		//printf("Self edges set to true\n");
		selfEdgeCapacity = size;
	}

	//One bit is set (true) for each attached point, or unset (false) for unattached points:
	bool attachedPoints[size] = {};
	//The MST is expanded starting with the last point in the data set:
	unsigned int currentPoint = size - 1;
	attachedPoints[size - 1] = true;

	//Each point has a current neighbor point in the tree, and a current nearest distance:
	vector<int> nearestMRDNeighbors(size - 1 + selfEdgeCapacity);
	vector<double> nearestMRDDistances(size - 1 + selfEdgeCapacity, numeric_limits<double>::max());

	//Create an array for vertices in the tree that each point attached to:
	vector<int> otherVertexIndices(size - 1 + selfEdgeCapacity);

	//Continue attaching points to the MST until all points are attached:
	for(uint numAttachedPoints = 1; numAttachedPoints < size; numAttachedPoints++){
		int nearestMRDPoint = -1;
		double nearestMRDDistance = numeric_limits<double>::max();

		//Iterate through all unattached points, updating distances using the current point:
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
		for (unsigned int neighbor = 0; neighbor < size; neighbor++) {
			if (currentPoint == neighbor) {
				continue;
			}

			if (attachedPoints[neighbor]) {
				continue;
			}

			double mutualReachabiltiyDistance = distanceFunction.getDistance(neighbor, currentPoint);

			if (coreDistances[currentPoint] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[currentPoint];
			}

			if (coreDistances[neighbor] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = coreDistances[neighbor];
			}

			if (mutualReachabiltiyDistance < nearestMRDDistances[neighbor]) {
				nearestMRDDistances[neighbor] = mutualReachabiltiyDistance;
				nearestMRDNeighbors[neighbor] = currentPoint;
			}

			//Check if the unattached point being updated is the closest to the tree:
			if (nearestMRDDistances[neighbor] <= nearestMRDDistance) {
				nearestMRDDistance = nearestMRDDistances[neighbor];
				nearestMRDPoint = neighbor;
			}

		}

		//Attach the closest point found in this iteration to the tree:
		attachedPoints[nearestMRDPoint] = true;
		otherVertexIndices[numAttachedPoints] = numAttachedPoints;
		currentPoint = nearestMRDPoint;

	}


	//If necessary, attach self edges:
	if (selfEdges) {

#ifdef USE_OPENMP
#pragma omp parallel for
#endif
		for (uint i = size - 1; i < size * 2 - 1; i++) {
			int vertex = i - (size - 1);
			nearestMRDNeighbors[i] = vertex;
			otherVertexIndices[i] = vertex;
			nearestMRDDistances[i] = coreDistances[vertex];
			//printf("At %d coreDistances[%d] = %f\n", i, vertex, coreDistances[vertex]);
		}
	}

	mst = UndirectedGraph(size, nearestMRDNeighbors, otherVertexIndices, nearestMRDDistances);

}

/**
 * Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * @param clusters A list of Clusters forming a cluster tree
 * @return true if there are any clusters with infinite stability, false otherwise
 */
template <class T1>
bool hdbscan<T1>::propagateTree() {

	map<int, Cluster*> clustersToExamine;

	vector<bool> addedToExaminationList(clusters.size());
	bool infiniteStability = false;

	//Find all leaf clusters in the cluster tree:
//#ifndef USE_OPENMP
//#pragma omp parallel for
//#endif
	for(uint i = 0; i < clusters.size(); i++){
		Cluster* cluster = clusters[i];
		if (cluster != NULL && !cluster->hasKids()) {
			//if()
			clustersToExamine.insert(
					pair<int, Cluster*>(cluster->getLabel(), cluster));
			addedToExaminationList[cluster->getLabel()] = true;
		}
	}

	//Iterate through every cluster, propagating stability from children to parents:
	while (!clustersToExamine.empty()) {
		map<int, Cluster*>::reverse_iterator itr = clustersToExamine.rbegin();
		pair<int, Cluster*> p = *itr;
		Cluster* currentCluster = p.second;
		clustersToExamine.erase(p.first);
		currentCluster->propagate();

		if (currentCluster->getStability() == numeric_limits<double>::max())
			infiniteStability = true;

		if (currentCluster->getParent() != NULL) {
			Cluster* parent = currentCluster->getParent();

			if (!addedToExaminationList[parent->getLabel()]) {
				clustersToExamine.insert(
						pair<int, Cluster*>(parent->getLabel(), parent));
				addedToExaminationList[parent->getLabel()] = true;
			}
		}
	}


	if (infiniteStability)
		printf("%s\n", getWarningMessage().c_str());
	return infiniteStability;
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
template <class T1>
void hdbscan<T1>::calculateOutlierScores(vector<double>* pointNoiseLevels,
		vector<int>* pointLastClusters, bool infiniteStability) {

	double* coreDistances = distanceFunction.getCoreDistances();
	int numPoints = pointNoiseLevels->size();
	//printf("Creating outlierScores\n");
	outlierScores.reserve(numPoints);

	for (int i = 0; i < numPoints; i++) {

		int tmp = (*pointLastClusters)[i];

		Cluster* c = clusters[tmp];
		double epsilon_max =
				c->getPropagatedLowestChildDeathLevel();
		double epsilon = (*pointNoiseLevels)[i];

		double score = 0;
		if (epsilon != 0) {
			score = 1 - (epsilon_max / epsilon);
		}
		outlierScores.push_back(OutlierScore(score, coreDistances[i], i));
	}
	//Sort the outlier scores:
	std::sort(outlierScores.begin(), outlierScores.end());
}

// ------------------------------ PRIVATE METHODS ------------------------------

/**
 * Removes the set of points from their parent Cluster, and creates a new Cluster, provided the
 * clusterId is not 0 (noise).
 * @param points The set of points to be in the new Cluster
 * @param clusterLabels An array of cluster labels, which will be modified
 * @param parentCluster The parent Cluster of the new Cluster being created
 * @param clusterLabel The label of the new Cluster
 * @param edgeWeight The edge weight at which to remove the points from their previous Cluster
 * @return The new Cluster, or NULL if the clusterId was 0
 */
template <class T1>
Cluster* hdbscan<T1>::createNewCluster(set<int>* points, vector<int>* clusterLabels,
		Cluster* parentCluster, int clusterLabel, double edgeWeight) {

	for (set<int>::iterator it = points->begin(); it != points->end(); ++it) {
		int idx = *it;
		(*clusterLabels)[idx] = clusterLabel;
	}

	parentCluster->detachPoints(points->size(), edgeWeight);

	if (clusterLabel != 0) {

		int s = (int) points->size();
		return new Cluster(clusterLabel, parentCluster, edgeWeight, s);
	}

	else {
		parentCluster->addPointsToVirtualChildCluster(*points);
		return NULL;
	}
}

/**
 * Calculates the number of constraints satisfied by the new clusters and virtual children of the
 * parents of the new clusters.
 * @param newClusterLabels Labels of new clusters
 * @param clusters An vector of clusters
 * @param constraints An vector of constraints
 * @param clusterLabels an array of current cluster labels for points
 */
template <class T1>
void hdbscan<T1>::calculateNumConstraintsSatisfied(set<int>& newClusterLabels, vector<int>& currentClusterLabels) {

	if (constraints.empty()) {
		return;
	}

	bool contains;
	vector<Cluster*> parents;

	for (set<int>::iterator it = newClusterLabels.begin();
			it != newClusterLabels.end(); ++it) {
		Cluster* parent = clusters[*it]->getParent();

		contains = find(parents.begin(), parents.end(), parent)
				!= parents.end();
		if (!contains)
			parents.push_back(parent);
	}

	for (vector<Constraint*>::iterator it = constraints.begin(); it != constraints.end(); ++it) {
		Constraint* constraint = *it;
		int labelA = currentClusterLabels[constraint->getPointA()];
		int labelB = currentClusterLabels[constraint->getPointB()];

		if (constraint->getType() == MUST_LINK && labelA == labelB) {
			if (newClusterLabels.find(labelA) != newClusterLabels.end()) {
				clusters[labelA]->addConstraintsSatisfied(2);
			}
		} else if (constraint->getType() == CANNOT_LINK
				&& (labelA != labelB || labelA == 0)) {

			contains = newClusterLabels.find(labelA) != newClusterLabels.end();
			if (labelA != 0 && contains) {
				clusters[labelA]->addConstraintsSatisfied(1);
			}

			contains = newClusterLabels.find(labelB) != newClusterLabels.end();
			if (labelB != 0 && contains) {
				clusters[labelB]->addConstraintsSatisfied(1);
			}

			if (labelA == 0) {
				for (vector<Cluster*>::iterator it = parents.begin();
						it != parents.end(); ++it) {
					Cluster* parent = *it;
					if (parent->virtualChildClusterContaintsPoint(
							constraint->getPointA())) {
						parent->addVirtualChildConstraintsSatisfied(1);
						break;
					}
				}
			}

			if (labelB == 0) {
				for (vector<Cluster*>::iterator it = parents.begin();
						it != parents.end(); ++it) {
					Cluster* parent = *it;
					if (parent->virtualChildClusterContaintsPoint(
							constraint->getPointB())) {
						parent->addVirtualChildConstraintsSatisfied(1);
						break;
					}
				}
			}
		}
	}

	for (vector<Cluster*>::iterator it = parents.begin(); it != parents.end();
			++it) {
		Cluster* parent = *it;
		parent->releaseVirtualChildCluster();
	}
}

template <class T1>
vector<int>& hdbscan<T1>::getClusterLabels() {
	return clusterLabels;
}

template <class T1>
map<int, double>& hdbscan<T1>::getClusterStabilities(){
	return clusterStabilities;
}


template <class T1>
vector<Cluster*>& hdbscan<T1>::getClusters() {
	return clusters;
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
 * @param constraints An optional ArrayList of Constraints to calculate cluster constraint satisfaction
 * @param hierarchyOutputFile The path to the hierarchy output file
 * @param treeOutputFile The path to the cluster tree output file
 * @param delimiter The delimiter to be used while writing both files
 * @param pointNoiseLevels A double[] to be filled with the levels at which each point becomes noise
 * @param pointLastClusters An int[] to be filled with the last label each point had before becoming noise
 * @return The cluster tree
 * @throws IOException If any errors occur opening or writing to the files
 */
template <class T1>
void hdbscan<T1>::computeHierarchyAndClusterTree(bool compactHierarchy, vector<double>* pointNoiseLevels, vector<int>* pointLastClusters) {

	//mst.print();
	int lineCount = 0; // Indicates the number of lines written into
								// hierarchyFile.

	//The current edge being removed from the MST:
	int currentEdgeIndex = mst.getNumEdges() - 1;
	//hierarchy = new map<long, vector<int>*>();

	int nextClusterLabel = 2;
	bool nextLevelSignificant = true;
	//The previous and current cluster numbers of each point in the data set:
	vector<int> previousClusterLabels(mst.getNumVertices(), 1);
	vector<int> currentClusterLabels(mst.getNumVertices(), 1);

	//A list of clusters in the cluster tree, with the 0th cluster (noise) null:
	clusters.push_back(NULL);
	clusters.push_back(new Cluster(1, NULL, NAN, mst.getNumVertices()));


	if (!constraints.empty()) {//Calculate number of constraints satisfied for cluster 1:
		set<int> clusterOne;
		clusterOne.insert(1);
		calculateNumConstraintsSatisfied(clusterOne, currentClusterLabels);
	}

	//

	//Sets for the clusters and vertices that are affected by the edge(s) being removed:
	set<int> affectedClusterLabels;
	set<int> affectedVertices;

	while (currentEdgeIndex >= 0) {

		double currentEdgeWeight = mst.getEdgeWeightAtIndex(currentEdgeIndex);
		vector<Cluster*> newClusters;
		//Remove all edges tied with the current edge weight, and store relevant clusters and vertices:
		while (currentEdgeIndex >= 0 && mst.getEdgeWeightAtIndex(currentEdgeIndex) == currentEdgeWeight) {

			int firstVertex = mst.getFirstVertexAtIndex(currentEdgeIndex);
			int secondVertex = mst.getSecondVertexAtIndex(currentEdgeIndex);

			mst.removeEdge(firstVertex, secondVertex);
			if (currentClusterLabels[firstVertex] == 0) {
				currentEdgeIndex--;
				continue;
			}

			affectedVertices.insert(firstVertex);
			affectedVertices.insert(secondVertex);

			affectedClusterLabels.insert(currentClusterLabels[firstVertex]);

			currentEdgeIndex--;
		}

		//Check each cluster affected for a possible split:
		while (!affectedClusterLabels.empty()) {
			set<int>::reverse_iterator it = affectedClusterLabels.rbegin();
			int examinedClusterLabel = *it;
			affectedClusterLabels.erase(examinedClusterLabel);
			set<int> examinedVertices;// = new set<int>();

			//Get all affected vertices that are members of the cluster currently being examined:
			for (set<int>::iterator itr = affectedVertices.begin();
					itr != affectedVertices.end(); ++itr) {
				int n = *itr;
				if (currentClusterLabels[n] == examinedClusterLabel) {
					examinedVertices.insert(n);
					affectedVertices.erase(n);
				}
			}

			set<int> firstChildCluster;// = NULL;
			vector<int> unexploredFirstChildClusterPoints;// = NULL;
			int numChildClusters = 0;

			/* Check if the cluster has split or shrunk by exploring the graph from each affected
			 * vertex.  If there are two or more valid child clusters (each has >= minClusterSize
			 * points), the cluster has split.
			 * Note that firstChildCluster will only be fully explored if there is a cluster
			 * split, otherwise, only spurious components are fully explored, in order to label
			 * them noise.
			 */
			while (!examinedVertices.empty()) {

				//TODO Clean up this
				set<int> constructingSubCluster;// = new set<int>();
				vector<int> unexploredSubClusterPoints;// = new vector<int>();

				bool anyEdges = false;
				bool incrementedChildCount = false;

				set<int>::reverse_iterator itr = affectedClusterLabels.rbegin();
				itr = examinedVertices.rbegin();
				int rootVertex = *itr;
				std::pair<std::set<int>::iterator, bool> p = constructingSubCluster.insert(rootVertex);

				unexploredSubClusterPoints.push_back(rootVertex);
				examinedVertices.erase(rootVertex);

				//Explore this potential child cluster as long as there are unexplored points:
				while (!unexploredSubClusterPoints.empty()) {
					int vertexToExplore = *(unexploredSubClusterPoints.begin());
					unexploredSubClusterPoints.erase(unexploredSubClusterPoints.begin());
					vector<int>* v = mst.getEdgeListForVertex(vertexToExplore);
					for (vector<int>::iterator itr = v->begin();
							itr != v->end(); ++itr) {
						int neighbor = *itr;
						anyEdges = true;

						p = constructingSubCluster.insert(neighbor);
						if (p.second) {
							unexploredSubClusterPoints.push_back(neighbor);
							examinedVertices.erase(neighbor);
						}
					}

					//Check if this potential child cluster is a valid cluster:
					if (!incrementedChildCount && constructingSubCluster.size() >= minPoints && anyEdges) {
						incrementedChildCount = true;
						numChildClusters++;

						//If this is the first valid child cluster, stop exploring it:
						if (firstChildCluster.empty()) {
							firstChildCluster.insert(constructingSubCluster.begin(), constructingSubCluster.end());
							unexploredFirstChildClusterPoints.insert(unexploredFirstChildClusterPoints.end(), unexploredSubClusterPoints.begin(), unexploredSubClusterPoints.end());
							break;
						}
					}
				}

				//If there could be a split, and this child cluster is valid:
				if (numChildClusters >= 2
						&& constructingSubCluster.size() >= minPoints
						&& anyEdges) {
					//Check this child cluster is not equal to the unexplored first child cluster:
					it = firstChildCluster.rbegin();
					int firstChildClusterMember = *it;
					if (constructingSubCluster.find(firstChildClusterMember)
							!= constructingSubCluster.end()) {
						numChildClusters--;
					}

					//Otherwise, create a new cluster:
					else {

						Cluster* newCluster = createNewCluster(
								&constructingSubCluster, &currentClusterLabels,
								clusters[examinedClusterLabel],
								nextClusterLabel, currentEdgeWeight);
						newClusters.push_back(newCluster);
						clusters.push_back(newCluster);

						nextClusterLabel++;
					}
				}

				//If this child cluster is not valid cluster, assign it to noise:
				else if (constructingSubCluster.size() < minPoints
						|| !anyEdges) {

					createNewCluster(
							&constructingSubCluster, &currentClusterLabels,
							clusters[examinedClusterLabel], 0,
							currentEdgeWeight);

					for (set<int>::iterator itr = constructingSubCluster.begin(); itr != constructingSubCluster.end(); ++itr) {
						int point = *itr;
						(*pointNoiseLevels)[point] = currentEdgeWeight;
						(*pointLastClusters)[point] = examinedClusterLabel;
					}
				}

			}

			//Finish exploring and cluster the first child cluster if there was a split and it was not already clustered:
			if (numChildClusters >= 2
					&& currentClusterLabels[*(firstChildCluster.begin())]
							== examinedClusterLabel) {

				while (!unexploredFirstChildClusterPoints.empty()) {
					vector<int>::iterator it =
							unexploredFirstChildClusterPoints.begin();
					int vertexToExplore = *it;
					unexploredFirstChildClusterPoints.erase(
							unexploredFirstChildClusterPoints.begin());
					vector<int>* v = mst.getEdgeListForVertex(vertexToExplore);

					for (vector<int>::iterator itr = v->begin();
							itr != v->end(); ++itr) {
						int neighbor = *itr;
						std::pair<std::set<int>::iterator, bool> p =
								firstChildCluster.insert(neighbor);
						if (p.second) {
							unexploredFirstChildClusterPoints.push_back(neighbor);
						}

					}
				}

				Cluster* newCluster = createNewCluster(&firstChildCluster,
						&currentClusterLabels, clusters[examinedClusterLabel],
						nextClusterLabel, currentEdgeWeight);
				newClusters.push_back(newCluster);
				//printf("Finish exploring %d\n", newCluster);
				clusters.push_back(newCluster);
				nextClusterLabel++;
			}
		}

		if (!compactHierarchy || nextLevelSignificant || !newClusters.empty()) {

			lineCount++;

			hierarchy.insert(pair<long, vector<int>>(lineCount, vector<int>(previousClusterLabels.begin(), previousClusterLabels.end())));
		}

		// Assign file offsets and calculate the number of constraints
					// satisfied:
		set<int> newClusterLabels;
		for (vector<Cluster*>::iterator itr = newClusters.begin(); itr != newClusters.end(); ++itr) {
			Cluster* newCluster = *itr;

			newCluster->setOffset(lineCount);
			newClusterLabels.insert(newCluster->getLabel());
		}

		if (!newClusterLabels.empty()){
			calculateNumConstraintsSatisfied(newClusterLabels, currentClusterLabels);
		}

		for (uint i = 0; i < previousClusterLabels.size(); i++) {

			previousClusterLabels[i] = currentClusterLabels[i];
		}

		if (newClusters.empty()){
			nextLevelSignificant = false;
		} else{
			nextLevelSignificant = true;
		}

	}

	vector<int> labels;
	// Write out the final level of the hierarchy (all points noise):
	for (uint i = 0; i < previousClusterLabels.size() - 1; i++) {
		labels.push_back(0);
	}
	labels.push_back(0);
	hierarchy.insert(pair<long, vector<int>>(0, labels));
	lineCount++;

}
uint getDatasetSize(int rows, int cols, bool rowwise){
    if(rowwise){
        return rows;
    } else{
        return rows * cols;
    }
}

template <class T1>
void hdbscan<T1>::run(vector<T1>& dataset){

    this->run(dataset, (int)dataset.size(), 1, false);
}

template <class T1>
void hdbscan<T1>::run(vector<T1>& dataset, int rows, int cols, bool rowwise){
    numPoints = getDatasetSize(rows, cols, rowwise);
    this->run();
}

template <class T1>
void hdbscan<T1>::run(T1* dataset, int size){
    this->run(dataset, size, 1, false);
}

template <class T1>
void hdbscan<T1>::run(T1* dataset, int rows, int cols, bool rowwise){
    numPoints = getDatasetSize(rows, cols, rowwise);
    this->distanceFunction.computeDistance(dataset, rows, cols, true, minPoints-1);
    this->run();
}


template <class T1>
void hdbscan<T1>::run() {

	constructMST();
	mst.quicksortByEdgeWeight();
	vector<double> pointNoiseLevels (numPoints);
	vector<int> pointLastClusters(numPoints);
	computeHierarchyAndClusterTree(false, &pointNoiseLevels, &pointLastClusters);
	bool infiniteStability = propagateTree();
	findProminentClusters(infiniteStability);
}

template <class T1>
void hdbscan<T1>::findProminentClusters(bool infiniteStability){
	vector<Cluster*>* solution = clusters[1]->getPropagatedDescendants();

	clusterLabels = findProminentClusters(infiniteStability, solution);
}

/**
 * Produces a flat clustering result using constraint satisfaction and cluster stability, and
 * returns an array of labels.  propagateTree() must be called before calling this method.
 * @param infiniteStability true if there are any clusters with infinite stability, false otherwise
 */
template <class T1>
vector<int> hdbscan<T1>::findProminentClusters(bool infiniteStability, vector<Cluster*>* solution) {


	clusterStabilities.insert(pair<int, double>(0, 0.0f));

	vector<int> labels(numPoints, 0);

	map<long, vector<int> > significant;
	set<int> toInspect;
//#pragma omp parallel for
	for (vector<Cluster*>::iterator itr = solution->begin(); itr != solution->end(); ++itr) {
		Cluster* cluster = *itr;
		if (cluster != NULL) {
			vector<int>* clusterList = &significant[cluster->getOffset()];
			if (significant.find(cluster->getOffset()) == significant.end()) {
				clusterList = new vector<int>();
				significant.insert(pair<long, vector<int> >(cluster->getOffset(), *clusterList));
			}

			clusterList->push_back(cluster->getLabel());

		}
	}

	while(!significant.empty()){
		pair<long, vector<int> > p = *(significant.begin());
		significant.erase(significant.begin());
		vector<int> clusterList = p.second;
		long offset = p.first;
		vector<int>& hpSecond = hierarchy[offset+1];

		for(uint i = 0; i < hpSecond.size(); i++){
			int label = hpSecond[i];
			vector<int>::iterator it = find(clusterList.begin(), clusterList.end(), label);
			if(it != clusterList.end()){
				labels[i] = label;

			}
		}
	}

	for(uint d = 0; d < labels.size(); ++d){
		for (vector<Cluster*>::iterator itr = solution->begin(); itr != solution->end(); ++itr) {
			Cluster* cluster = *itr;
			if(cluster->getLabel() == labels[d]){
				clusterStabilities.insert(pair<int, double>(cluster->getLabel(), cluster->getStability()));
			}
		}
	}

	return labels;
}

template <class T1>
bool hdbscan<T1>::compareClusters(Cluster* one, Cluster* two){

	return one == two;
}

template <class T1>
double* hdbscan<T1>::getCoreDistances(){
	return this->distanceFunction.getCoreDistances();
}

template <class T1>
DistanceCalculator<T1>& hdbscan<T1>::getDistanceFunction(){
	return distanceFunction;
}

template <class T1>
double hdbscan<T1>::getDistance(uint i, uint j){
	return distanceFunction.getDistance(i, j);
}

template <class T1>
void hdbscan<T1>::clean(){
	this->clusterLabels.clear();
	this->clusterStabilities.clear();
	//this->distanceFunction.clean();
	this->hierarchy.clear();
	this->mst.clean();
	this->outlierScores.clear();

	for (uint i = 0; i < constraints.size(); ++i) {
		delete constraints[i];
	}

	for(uint i = 0; i < clusters.size(); ++i){
		delete clusters[i];
	}

	this->clusters.clear();
	this->constraints.clear();
}

template class hdbscan<float>;
template class hdbscan<double>;
template class hdbscan<int>;
template class hdbscan<long>;
template class hdbscan<unsigned int>;
template class hdbscan<unsigned long>;

}
/* namespace clustering */
