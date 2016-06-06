/*
 * hdbscan.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "hdbscan.hpp"
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace clustering;
using namespace clustering::distance;
using namespace std::chrono;
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

HDBSCAN::HDBSCAN(calculator cal, uint minPoints, uint minClusterSize) {
	this->minPoints = minPoints;
	this->minClusterSize = minClusterSize;
	distanceFunction = new DistanceCalculator(cal);
	selfEdges = true;
	constraints = NULL;
	clusterLabels = NULL;
	dataSet = NULL;
	mst = NULL;
	clusters = NULL;
	outlierScores = NULL;
	coreDistances = NULL;
	hierarchy = NULL;
	numPoints = 0;
}
HDBSCAN::HDBSCAN(vector<vector<double> >* dataSet, calculator cal,
		uint minPoints, uint minClusterSize) {

	this->minClusterSize = minClusterSize;
	this->minPoints = minPoints;
	distanceFunction = new DistanceCalculator(cal);
	this->dataSet = dataSet;
	selfEdges = true;
	constraints = NULL;
	clusterLabels = NULL;
	mst = NULL;
	clusters = NULL;
	outlierScores = NULL;
	hierarchy = NULL;
	calculateCoreDistances();
	numPoints = dataSet->size();
}

HDBSCAN::HDBSCAN(string fileName, calculator cal, uint minPoints,
		uint minClusterSize) {
	this->minPoints = minPoints;
	this->minClusterSize = minClusterSize;
	distanceFunction = new DistanceCalculator(cal);
	selfEdges = true;
	constraints = NULL;
	clusterLabels = NULL;
	mst = NULL;
	clusters = NULL;
	outlierScores = NULL;
	hierarchy = NULL;
	readInDataSet(fileName);
	calculateCoreDistances();
	numPoints = dataSet->size();
}

HDBSCAN::HDBSCAN(string dataFileName, string constraintFileName, calculator cal,
		uint minPoints, uint minClusterSize) {
	this->minPoints = minPoints;
	this->minClusterSize = minClusterSize;
	distanceFunction = new DistanceCalculator(cal);
	selfEdges = true;
	constraints = NULL;
	clusterLabels = NULL;
	mst = NULL;
	clusters = NULL;
	outlierScores = NULL;
	hierarchy = NULL;
	readInDataSet(dataFileName);
	readInConstraints(constraintFileName);
	calculateCoreDistances();
	numPoints = dataSet->size();
}

HDBSCAN::~HDBSCAN() {

	if (constraints != NULL) {
		delete constraints;
	}

	if (distanceFunction != NULL) {
		delete distanceFunction;
	}

	if (clusterLabels != NULL) {
		delete clusterLabels;
	}

	if (mst != NULL) {
		delete mst;
	}

	if (clusters != NULL) {
		delete clusters;
	}

	if (outlierScores != NULL) {
		delete outlierScores;
	}

	if (coreDistances != NULL) {
		delete coreDistances;
	}
}

/**
 * Reads in the input data set from the file given, assuming the delimiter separates attributes
 * for each data point, and each point is given on a separate line.  Error messages are printed
 * if any part of the input is improperly formatted.
 * @param fileName The path to the input file
 * @param delimiter A regular expression that separates the attributes of each point
 * @return A vector<double>[] where index [i][j] indicates the jth attribute of data point i
 * @throws IOException If any errors occur opening or reading from the file
 */
void HDBSCAN::readInDataSet(string fileName) {

	dataSet = new vector<vector<double> >();
	std::ifstream inFile(fileName);
	string item;

	while (inFile) {
		if (!getline(inFile, item)) {
			break;
		}

		istringstream ss(item);
		vector<double> line;

		while (ss) {
			string s;
			if (!getline(ss, s, ',')) {
				break;
			}
			line.push_back(atof(s.c_str()));
		}
		dataSet->push_back(line);
	}
}

/**
 * Reads in constraints from the file given, assuming the delimiter separates the points involved
 * in the constraint and the type of the constraint, and each constraint is given on a separate
 * line.  Error messages are printed if any part of the input is improperly formatted.
 * @param fileName The path to the input file
 * @param delimiter A regular expression that separates the points and type of each constraint
 * @return An vector of Constraints
 * @throws IOException If any errors occur opening or reading from the file
 */
void HDBSCAN::readInConstraints(string fileName) {

	constraints = new vector<Constraint*>();
	std::ifstream inFile(fileName);
	string item;
	while (getline(inFile, item, ',')) {
		CONSTRAINT_TYPE type;
		int pointA, pointB;

		pointA = atoi(item.c_str());
		getline(inFile, item, ',');
		pointB = atoi(item.c_str());
		getline(inFile, item, '\n');
		if (item == MUST_LINK_TAG) {
			type = MUST_LINK;
		} else if (item == CANNOT_LINK_TAG) {
			type = CANNOT_LINK;
		}

		constraints->push_back(new Constraint(pointA, pointB, type));
	}

}

/**
 * Calculates the core distances for each point in the data set, given some value for k.
 * @param dataSet A vector<vector<double> > where index [i][j] indicates the jth attribute of data point i
 * @param k Each point's core distance will be it's distance to the kth nearest neighbor
 * @param distanceFunction A DistanceCalculator to compute distances between points
 * @return An array of core distances
 */
void HDBSCAN::calculateCoreDistances() {
	int numNeighbors = minPoints - 1;
	coreDistances = new vector<double>(dataSet->size(), 0);

	if (minPoints == 1) {
		return;
	}

	uint size = dataSet->size();

	for (unsigned int point = 0; point < size; point++) {
		vector<double> kNNDistances(numNeighbors, numeric_limits<double>::max()); //Sorted nearest distances found so far

		for (unsigned int neighbor = 0; neighbor < size; neighbor++) {
			if (point == neighbor) {
				continue;
			}

			vector<double> attributesOne = dataSet[0][point];
			vector<double> attributesTwo = dataSet[0][neighbor];


			double distance = distanceFunction->computeDistance(&attributesOne,
					&attributesTwo);

			//Check at which position in the nearest distances the current distance would fit:
			int neighborIndex = numNeighbors;
			while (neighborIndex >= 1
					&& distance < kNNDistances[neighborIndex - 1]) {
				neighborIndex--;
			}

			//Shift elements in the array to make room for the current distance:
			if (neighborIndex < numNeighbors) {
				for (int shiftIndex = numNeighbors - 1;
						shiftIndex > neighborIndex; shiftIndex--) {
					kNNDistances[shiftIndex] = kNNDistances[shiftIndex - 1];
				}
				kNNDistances[neighborIndex] = distance;
			}
		}
		(*coreDistances)[point] = kNNDistances[numNeighbors - 1];
	}

}

/**
 * Constructs the minimum spanning tree of mutual reachability distances for the data set, given
 * the core distances for each point.
 * @param dataSet A vector<vector<double> > where index [i][j] indicates the jth attribute of data point i
 * @param coreDistances An array of core distances for each data point
 * @param selfEdges If each point should have an edge to itself with weight equal to core distance
 * @param distanceFunction A DistanceCalculator to compute distances between points
 */
void HDBSCAN::constructMST() {

	int selfEdgeCapacity = 0;
	uint size = dataSet->size();
	if (selfEdges)
		selfEdgeCapacity = size;

	//One bit is set (true) for each attached point, or unset (false) for unattached points:
	bool attachedPoints[selfEdgeCapacity] = {};

	//Each point has a current neighbor point in the tree, and a current nearest distance:
	vector<int>* nearestMRDNeighbors = new vector<int>(size - 1 + selfEdgeCapacity);
	vector<double>* nearestMRDDistances = new vector<double>(size - 1 + selfEdgeCapacity, numeric_limits<double>::max());

	//Create an array for vertices in the tree that each point attached to:
	vector<int>* otherVertexIndices = new vector<int>(size - 1 + selfEdgeCapacity);

	//The MST is expanded starting with the last point in the data set:
	unsigned int currentPoint = size - 1;
	int numAttachedPoints = 1;
	attachedPoints[size - 1] = true;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//Continue attaching points to the MST until all points are attached:
	while (numAttachedPoints < (int)size) {
		int nearestMRDPoint = -1;
		double nearestMRDDistance = numeric_limits<double>::max();

		//Iterate through all unattached points, updating distances using the current point:
		for (unsigned int neighbor = 0; neighbor < size;
				neighbor++) {
			if (currentPoint == neighbor) {
				continue;
			}

			if (attachedPoints[neighbor]) {
				continue;
			}

			double distance = distanceFunction->computeDistance(
					&(*dataSet)[currentPoint], &(*dataSet)[neighbor]);

			double mutualReachabiltiyDistance = distance;
			if ((*coreDistances)[currentPoint] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = (*coreDistances)[currentPoint];
			}

			if ((*coreDistances)[neighbor] > mutualReachabiltiyDistance) {
				mutualReachabiltiyDistance = (*coreDistances)[neighbor];
			}

			if (mutualReachabiltiyDistance < (*nearestMRDDistances)[neighbor]) {
				(*nearestMRDDistances)[neighbor] = mutualReachabiltiyDistance;
				(*nearestMRDNeighbors)[neighbor] = currentPoint;
			}

			//Check if the unattached point being updated is the closest to the tree:
			if ((*nearestMRDDistances)[neighbor] <= nearestMRDDistance) {
				nearestMRDDistance = (*nearestMRDDistances)[neighbor];
				nearestMRDPoint = neighbor;
			}

		}

		//Attach the closest point found in this iteration to the tree:
		attachedPoints[nearestMRDPoint] = true;
		(*otherVertexIndices)[numAttachedPoints] = numAttachedPoints;
		numAttachedPoints++;
		currentPoint = nearestMRDPoint;

	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "construct mst while loop time : " << duration << endl;

	//If necessary, attach self edges:
	if (selfEdges) {

		for (uint i = size - 1; i < size * 2 - 1; i++) {
			int vertex = i - (dataSet->size() - 1);
			(*nearestMRDNeighbors)[i] = vertex;
			(*otherVertexIndices)[i] = vertex;
			(*nearestMRDDistances)[i] = (*coreDistances)[vertex];
		}
	}

	mst = new UndirectedGraph(size, nearestMRDNeighbors,
			otherVertexIndices, nearestMRDDistances);
}

/**
 * Propagates constraint satisfaction, stability, and lowest child death level from each child
 * cluster to each parent cluster in the tree.  This method must be called before calling
 * findProminentClusters() or calculateOutlierScores().
 * @param clusters A list of Clusters forming a cluster tree
 * @return true if there are any clusters with infinite stability, false otherwise
 */
bool HDBSCAN::propagateTree() {

	map<int, Cluster*> clustersToExamine;

	vector<bool> addedToExaminationList(clusters->size());
	bool infiniteStability = false;

	//Find all leaf clusters in the cluster tree:
	for (vector<Cluster*>::iterator itr = clusters->begin();
			itr != clusters->end(); ++itr) {
		Cluster* cluster = *itr;
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
void HDBSCAN::calculateOutlierScores(vector<double>* pointNoiseLevels,
		vector<int>* pointLastClusters, bool infiniteStability) {

	int numPoints = pointNoiseLevels->size();
	outlierScores = new vector<OutlierScore*>(numPoints);

	//Iterate through each point, calculating its outlier score:
	for (int i = 0; i < numPoints; i++) {
		double epsilon_max =
				(*clusters)[(*pointLastClusters)[i]]->getPropagatedLowestChildDeathLevel();
		double epsilon = (*pointNoiseLevels)[i];

		double score = 0;
		if (epsilon != 0) {
			score = 1 - (epsilon_max / epsilon);
		}

		outlierScores->push_back(
				new OutlierScore(score, (*coreDistances)[i], i));
	}

	//Sort the outlier scores:
	std::sort(outlierScores->begin(), outlierScores->end());
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
Cluster* HDBSCAN::createNewCluster(set<int>* points, vector<int>* clusterLabels,
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
		parentCluster->addPointsToVirtualChildCluster(points);
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
void HDBSCAN::calculateNumConstraintsSatisfied(set<int> newClusterLabels,
		vector<int> currentClusterLabels) {

	if (constraints == NULL) {
		return;
	}

	bool contains;
	vector<Cluster*> parents;

	for (set<int>::iterator it = newClusterLabels.begin();
			it != newClusterLabels.end(); ++it) {
		Cluster* parent = (*clusters)[*it]->getParent();

		contains = find(parents.begin(), parents.end(), parent)
				!= parents.end();
		if (!contains)
			parents.push_back(parent);
	}

	for (vector<Constraint*>::iterator it = constraints->begin();
			it != constraints->end(); ++it) {
		Constraint* constraint = *it;
		int labelA = currentClusterLabels[constraint->getPointA()];
		int labelB = currentClusterLabels[constraint->getPointB()];

		if (constraint->getType() == MUST_LINK && labelA == labelB) {
			if (newClusterLabels.find(labelA) != newClusterLabels.end()) {
				(*clusters)[labelA]->addConstraintsSatisfied(2);
			}
		} else if (constraint->getType() == CANNOT_LINK
				&& (labelA != labelB || labelA == 0)) {

			contains = newClusterLabels.find(labelA) != newClusterLabels.end();
			if (labelA != 0 && contains) {
				(*clusters)[labelA]->addConstraintsSatisfied(1);
			}

			contains = newClusterLabels.find(labelB) != newClusterLabels.end();
			if (labelB != 0 && contains) {
				(*clusters)[labelB]->addConstraintsSatisfied(1);
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

vector<int>* HDBSCAN::getClusterLabels() {
	return clusterLabels;
}

vector<vector<double> >* HDBSCAN::getDataSet() {
	return dataSet;
}

vector<Cluster*>* HDBSCAN::getClusters() {
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
void HDBSCAN::computeHierarchyAndClusterTree(bool compactHierarchy,
		vector<double>* pointNoiseLevels, vector<int>* pointLastClusters) {

	int lineCount = 0; // Indicates the number of lines written into
								// hierarchyFile.

	//The current edge being removed from the MST:
	int currentEdgeIndex = mst->getNumEdges() - 1;
	hierarchy = new map<long, vector<int>*>();

	int nextClusterLabel = 2;
	bool nextLevelSignificant = true;
	clusters = new vector<Cluster*>();
	//The previous and current cluster numbers of each point in the data set:
	vector<int>* previousClusterLabels = new vector<int>(mst->getNumVertices(), 1);
	vector<int>* currentClusterLabels = new vector<int>(mst->getNumVertices(), 1);

	//A list of clusters in the cluster tree, with the 0th cluster (noise) null:
	clusters->push_back(NULL);
	clusters->push_back(new Cluster(1, NULL, NAN, mst->getNumVertices()));

	//Calculate number of constraints satisfied for cluster 1:
	set<int>* clusterOne = new set<int>();
	clusterOne->insert(1);
	if (constraints != NULL && constraints->size() > 0) {
		calculateNumConstraintsSatisfied(*clusterOne, *currentClusterLabels);
	}

	//Sets for the clusters and vertices that are affected by the edge(s) being removed:
	set<int>* affectedClusterLabels = new set<int>();
	set<int>* affectedVertices = new set<int>();

	while (currentEdgeIndex >= 0) {

		double currentEdgeWeight = mst->getEdgeWeightAtIndex(currentEdgeIndex);
		vector<Cluster*>* newClusters = new vector<Cluster*>();
		//Remove all edges tied with the current edge weight, and store relevant clusters and vertices:
		while (currentEdgeIndex >= 0
				&& mst->getEdgeWeightAtIndex(currentEdgeIndex)
						== currentEdgeWeight) {
			int firstVertex = mst->getFirstVertexAtIndex(currentEdgeIndex);
			int secondVertex = mst->getSecondVertexAtIndex(currentEdgeIndex);

			mst->removeEdge(firstVertex, secondVertex);
			if ((*currentClusterLabels)[firstVertex] == 0) {
				currentEdgeIndex--;
				continue;
			}

			affectedVertices->insert(firstVertex);
			affectedVertices->insert(secondVertex);

			affectedClusterLabels->insert(
					(*currentClusterLabels)[firstVertex]);

			currentEdgeIndex--;
		}

		//Check each cluster affected for a possible split:
		while (!affectedClusterLabels->empty()) {
			set<int>::reverse_iterator it = affectedClusterLabels->rbegin();
			int examinedClusterLabel = *it;
			affectedClusterLabels->erase(examinedClusterLabel);
			set<int> *examinedVertices = new set<int>();

			//Get all affected vertices that are members of the cluster currently being examined:
			for (set<int>::iterator itr = affectedVertices->begin();
					itr != affectedVertices->end(); ++itr) {
				int n = *itr;
				if ((*currentClusterLabels)[n] == examinedClusterLabel) {
					examinedVertices->insert(n);
					affectedVertices->erase(n);
				}
			}

			set<int>* firstChildCluster = NULL;
			vector<int>* unexploredFirstChildClusterPoints = NULL;
			int numChildClusters = 0;

			/* Check if the cluster has split or shrunk by exploring the graph from each affected
			 * vertex.  If there are two or more valid child clusters (each has >= minClusterSize
			 * points), the cluster has split.
			 * Note that firstChildCluster will only be fully explored if there is a cluster
			 * split, otherwise, only spurious components are fully explored, in order to label
			 * them noise.
			 */
			while (!examinedVertices->empty()) {
				set<int>* constructingSubCluster = new set<int>();
				vector<int>* unexploredSubClusterPoints = new vector<int>();
				bool anyEdges = false;
				bool incrementedChildCount = false;

				set<int>::reverse_iterator itr =
						affectedClusterLabels->rbegin();
				itr = examinedVertices->rbegin();
				int rootVertex = *itr;
				std::pair<std::set<int>::iterator, bool> p =
						constructingSubCluster->insert(rootVertex);

				unexploredSubClusterPoints->push_back(rootVertex);
				examinedVertices->erase(rootVertex);

				//Explore this potential child cluster as long as there are unexplored points:
				while (!unexploredSubClusterPoints->empty()) {
					int vertexToExplore = *(unexploredSubClusterPoints->begin());
					unexploredSubClusterPoints->erase(
							unexploredSubClusterPoints->begin());
					vector<int>* v = mst->getEdgeListForVertex(vertexToExplore);
					for (vector<int>::iterator itr = v->begin();
							itr != v->end(); ++itr) {
						int neighbor = *itr;
						anyEdges = true;

						p = constructingSubCluster->insert(neighbor);
						if (p.second) {
							unexploredSubClusterPoints->push_back(neighbor);
							examinedVertices->erase(neighbor);
						}
					}

					//Check if this potential child cluster is a valid cluster:
					if (!incrementedChildCount
							&& constructingSubCluster->size() >= minClusterSize
							&& anyEdges) {
						incrementedChildCount = true;
						numChildClusters++;

						//If this is the first valid child cluster, stop exploring it:
						if (firstChildCluster == NULL) {
							firstChildCluster = constructingSubCluster;
							unexploredFirstChildClusterPoints =
									unexploredSubClusterPoints;
							break;
						}
					}
				}

				//If there could be a split, and this child cluster is valid:
				if (numChildClusters >= 2
						&& constructingSubCluster->size() >= minClusterSize
						&& anyEdges) {
					//Check this child cluster is not equal to the unexplored first child cluster:
					it = firstChildCluster->rbegin();
					int firstChildClusterMember = *it;
					if (constructingSubCluster->find(firstChildClusterMember)
							!= constructingSubCluster->end()) {
						numChildClusters--;
					}

					//Otherwise, create a new cluster:
					else {

						Cluster* newCluster = createNewCluster(
								constructingSubCluster, currentClusterLabels,
								(*clusters)[examinedClusterLabel],
								nextClusterLabel, currentEdgeWeight);
						//printf("Otherwise, create a new cluster: %d of label %d\n", newCluster, newCluster->getLabel());
						newClusters->push_back(newCluster);
						clusters->push_back(newCluster);
						nextClusterLabel++;
					}
				}

				//If this child cluster is not valid cluster, assign it to noise:
				else if (constructingSubCluster->size() < minClusterSize
						|| !anyEdges) {

					createNewCluster(
							constructingSubCluster, currentClusterLabels,
							(*clusters)[examinedClusterLabel], 0,
							currentEdgeWeight);

					for (set<int>::iterator itr = constructingSubCluster->begin(); itr != constructingSubCluster->end(); ++itr) {
						int point = *itr;
						(*pointNoiseLevels)[point] = currentEdgeWeight;
						(*pointLastClusters)[point] = examinedClusterLabel;
					}
				}
			}

			//Finish exploring and cluster the first child cluster if there was a split and it was not already clustered:
			if (numChildClusters >= 2
					&& (*currentClusterLabels)[*(firstChildCluster->begin())]
							== examinedClusterLabel) {

				while (!unexploredFirstChildClusterPoints->empty()) {
					vector<int>::iterator it =
							unexploredFirstChildClusterPoints->begin();
					int vertexToExplore = *it;
					unexploredFirstChildClusterPoints->erase(
							unexploredFirstChildClusterPoints->begin());
					vector<int>* v = mst->getEdgeListForVertex(vertexToExplore);

					for (vector<int>::iterator itr = v->begin();
							itr != v->end(); ++itr) {
						int neighbor = *itr;
						std::pair<std::set<int>::iterator, bool> p =
								firstChildCluster->insert(neighbor);
						if (p.second) {
							unexploredFirstChildClusterPoints->push_back(
									neighbor);
						}

					}
				}

				Cluster* newCluster = createNewCluster(firstChildCluster,
						currentClusterLabels, (*clusters)[examinedClusterLabel],
						nextClusterLabel, currentEdgeWeight);
				newClusters->push_back(newCluster);
				clusters->push_back(newCluster);
				nextClusterLabel++;
			}
		}

		//Write out the current level of the hierarchy:
		if (!compactHierarchy || nextLevelSignificant
				|| !newClusters->empty()) {

			lineCount++;

			hierarchy->insert(pair<long, vector<int>*>(lineCount, new vector<int>(previousClusterLabels->begin(), previousClusterLabels->end())));
		}

		// Assign file offsets and calculate the number of constraints
					// satisfied:
		set<int>* newClusterLabels = new set<int>();
		for (vector<Cluster*>::iterator itr = newClusters->begin(); itr != newClusters->end(); ++itr) {
			Cluster* newCluster = *itr;

			newCluster->setOffset(lineCount);
			newClusterLabels->insert(newCluster->getLabel());
		}

		if (!newClusterLabels->empty()){
			calculateNumConstraintsSatisfied(*newClusterLabels, *currentClusterLabels);
		}

		for (uint i = 0; i < previousClusterLabels->size(); i++) {
			(*previousClusterLabels)[i] = (*currentClusterLabels)[i];
		}
	}
	vector<int>* labels = new vector<int>();
	// Write out the final level of the hierarchy (all points noise):
	for (uint i = 0; i < previousClusterLabels->size() - 1; i++) {
		labels->push_back(0);
	}
	labels->push_back(0);
	hierarchy->insert(pair<long, vector<int>*>(0, labels));
	lineCount++;


}

void HDBSCAN::run(bool calcDistances) {
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	if (calcDistances) {
		calculateCoreDistances();
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "calculateCoreDistances time : " << duration << endl;

	t1 = high_resolution_clock::now();
	//Calculate minimum spanning tree:
	constructMST();
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "constructMST time : " << duration << endl;

	t1 = high_resolution_clock::now();
	mst->quicksortByEdgeWeight();
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "quicksortByEdgeWeight time : " << duration << endl;

	int numPoints = coreDistances->size();

	t1 = high_resolution_clock::now();
	// Remove references to unneeded objects:
	vector<vector<double> >().swap(*dataSet);

	vector<double>* pointNoiseLevels = new vector<double>(numPoints);
	vector<int>* pointLastClusters = new vector<int>(numPoints);
	//Compute hierarchy and cluster tree:
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "vector<vector<double> >().swap(*dataSet) time : " << duration << endl;

	t1 = high_resolution_clock::now();
	computeHierarchyAndClusterTree(true, pointNoiseLevels, pointLastClusters);

	//Remove references to unneeded objects:
	mst = NULL;
	//Propagate clusters:
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "computeHierarchyAndClusterTree time : " << duration << endl;

	t1 = high_resolution_clock::now();
	bool infiniteStability = propagateTree();

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "propagateTree time : " << duration << endl;

	t1 = high_resolution_clock::now();
	//Compute final flat partitioning:
	findProminentClusters(infiniteStability);

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "findProminentClusters time : " << duration << endl;

	t1 = high_resolution_clock::now();
	//Compute outlier scores for each point:
	calculateOutlierScores(pointNoiseLevels, pointLastClusters,
			infiniteStability);

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();

	cout << "calculateOutlierScores time : " << duration << endl;

	cout << endl;
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
void HDBSCAN::findProminentClusters(bool infiniteStability) {

	vector<Cluster*>* solution = (*clusters)[1]->getPropagatedDescendants();

	clusterLabels = new vector<int>(numPoints, 0);
	map<long, vector<int> > significant;
	set<int> toInspect;

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

	//printf("clusters size: %d and significant size: %d\n\n[", clusters->size(), significant.size());

	while(!significant.empty()){
		pair<long, vector<int> > p = *(significant.begin());
		significant.erase(significant.begin());
		vector<int> clusterList = p.second;
		long offset = p.first;
		hierarchy->size();
		vector<int>* hpSecond = (*hierarchy)[offset+1];

		for(uint i = 0; i < hpSecond->size(); i++){
			int label = (*hpSecond)[i];
			vector<int>::iterator it = find(clusterList.begin(), clusterList.end(), label);
			if(it != clusterList.end()){
				(*clusterLabels)[i] = label;
			}

		}
	}

}

bool HDBSCAN::compareClusters(Cluster* one, Cluster* two){

	return one == two;
}

}
/* namespace clustering */
