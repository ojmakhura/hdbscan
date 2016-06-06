/*
 * undirected_graph.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "undirected_graph.hpp"

#include <algorithm>

namespace clustering {

UndirectedGraph::~UndirectedGraph() {
	// TODO Auto-generated destructor stub
}

// ------------------------------ CONSTRUCTORS ------------------------------

/**
 * Constructs a new UndirectedGraph, including creating an edge list for each vertex from the
 * vertex arrays.  For an index i, verticesA[i] and verticesB[i] share an edge with weight
 * edgeWeights[i].
 * @param numVertices The number of vertices in the graph (indexed 0 to numVertices-1)
 * @param verticesA An array of vertices corresponding to the array of edges
 * @param verticesB An array of vertices corresponding to the array of edges
 * @param edgeWeights An array of edges corresponding to the arrays of vertices
 */
UndirectedGraph::UndirectedGraph(int numVertices, vector<int>* verticesA,
		vector<int>* verticesB, vector<double>* edgeWeights) {
	this->numVertices = numVertices;
	this->verticesA = verticesA;
	this->verticesB = verticesB;
	this->edgeWeights = edgeWeights;

	this->edges = new  vector<vector<int> >(numVertices);

	bool selfEdges = edgeWeights->size() == numVertices * 2 - 1;

	for (unsigned int i = 0; i < edgeWeights->size(); i++) {
		int vertexOne = this->verticesA[0][i];
		int vertexTwo = this->verticesB[0][i];
		(*this->edges)[vertexOne].push_back(vertexTwo);

		if (vertexOne != vertexTwo){
			(*this->edges)[vertexTwo].push_back(vertexOne);
		}
	}
}

// ------------------------------ PUBLIC METHODS ------------------------------

/**
 * Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
 * iterative and in-place.
 */
void UndirectedGraph::quicksortByEdgeWeight() {
	if (this->edgeWeights->size() <= 1)
		return;

	vector<int>* startIndexStack = new vector<int>(this->edgeWeights->size()/2);
	vector<int>* endIndexStack = new vector<int>(this->edgeWeights->size()/2);

	(*startIndexStack)[0] = 0;
	(*endIndexStack)[0] = this->edgeWeights->size() - 1;
	int stackTop = 0;

	while (stackTop >= 0) {
		int startIndex = (*startIndexStack)[stackTop];
		int endIndex = (*endIndexStack)[stackTop];
		stackTop--;

		int pivotIndex = this->selectPivotIndex(startIndex, endIndex);
		pivotIndex = this->partition(startIndex, endIndex, pivotIndex);

		if (pivotIndex > startIndex + 1) {
			(*startIndexStack)[stackTop + 1] = startIndex;
			(*endIndexStack)[stackTop + 1] = pivotIndex - 1;
			stackTop++;
		}

		if (pivotIndex < endIndex - 1) {
			(*startIndexStack)[stackTop + 1] = pivotIndex + 1;
			(*endIndexStack)[stackTop + 1] = endIndex;
			stackTop++;
		}
	}
}

// ------------------------------ PRIVATE METHODS ------------------------------

/**
 * Quicksorts the graph in the interval [startIndex, endIndex] by edge weight.
 * @param startIndex The lowest index to be included in the sort
 * @param endIndex The highest index to be included in the sort
 */
void UndirectedGraph::quicksort(int startIndex, int endIndex) {
	if (startIndex < endIndex) {
		int pivotIndex = this->selectPivotIndex(startIndex, endIndex);
		pivotIndex = this->partition(startIndex, endIndex, pivotIndex);
		this->quicksort(startIndex, pivotIndex - 1);
		this->quicksort(pivotIndex + 1, endIndex);
	}
}

/**
 * Returns a pivot index by finding the median of edge weights between the startIndex, endIndex,
 * and middle.
 * @param startIndex The lowest index from which the pivot index should come
 * @param endIndex The highest index from which the pivot index should come
 * @return A pivot index
 */
int UndirectedGraph::selectPivotIndex(int startIndex, int endIndex) {
	if (startIndex - endIndex <= 1)
		return startIndex;

	double first = (*edgeWeights)[startIndex];
	double middle = (*edgeWeights)[startIndex + (endIndex - startIndex) / 2];
	double last = (*edgeWeights)[endIndex];

	if (first <= middle) {
		if (middle <= last)
			return startIndex + (endIndex - startIndex) / 2;
		else if (last >= first)
			return endIndex;
		else
			return startIndex;
	} else {
		if (first <= last)
			return startIndex;
		else if (last >= middle)
			return endIndex;
		else
			return startIndex + (endIndex - startIndex) / 2;
	}
}

/**
 * Partitions the array in the interval [startIndex, endIndex] around the value at pivotIndex.
 * @param startIndex The lowest index to  partition
 * @param endIndex The highest index to partition
 * @param pivotIndex The index of the edge weight to partition around
 * @return The index position of the pivot edge weight after the partition
 */
int UndirectedGraph::partition(int startIndex, int endIndex, int pivotIndex) {
	double pivotValue = (*this->edgeWeights)[pivotIndex];
	this->swapEdges(pivotIndex, endIndex);
	int lowIndex = startIndex;

	for (int i = startIndex; i < endIndex; i++) {
		if ((*this->edgeWeights)[i] < pivotValue) {
			this->swapEdges(i, lowIndex);
			lowIndex++;
		}
	}

	this->swapEdges(lowIndex, endIndex);
	return lowIndex;
}

/**
 * Swaps the vertices and edge weights between two index locations in the graph.
 * @param indexOne The first index location
 * @param indexTwo The second index location
 */
void UndirectedGraph::swapEdges(int indexOne, int indexTwo) {
	if (indexOne != indexTwo){
		int tempVertexA = (*this->verticesA)[indexOne];
		int tempVertexB = (*this->verticesB)[indexOne];
		double tempEdgeDistance = (*this->edgeWeights)[indexOne];

		(*this->verticesA)[indexOne] = (*this->verticesA)[indexTwo];
		(*this->verticesB)[indexOne] = (*this->verticesB)[indexTwo];
		(*this->edgeWeights)[indexOne] = (*this->edgeWeights)[indexTwo];

		(*this->verticesA)[indexTwo] = tempVertexA;
		(*this->verticesB)[indexTwo] = tempVertexB;
		(*this->edgeWeights)[indexTwo] = tempEdgeDistance;
	}
}

// ------------------------------ GETTERS & SETTERS ------------------------------

int UndirectedGraph::getNumVertices() {
	return this->numVertices;
}

int UndirectedGraph::getNumEdges() {
	return this->edgeWeights->size();
}

int UndirectedGraph::getFirstVertexAtIndex(int index) {
	return (*this->verticesA)[index];
}

int UndirectedGraph::getSecondVertexAtIndex(int index) {
	return (*this->verticesB)[index];
}

double UndirectedGraph::getEdgeWeightAtIndex(int index) {
	return (*this->edgeWeights)[index];
}

vector<int>* UndirectedGraph::getEdgeListForVertex(int vertex) {
	return &(*this->edges)[vertex];
}

void UndirectedGraph::removeEdge(int va, int vb){
	// get the edge list for va
	// find position for vb in the edge list for va
	vector<int>* edgeList = getEdgeListForVertex(va);
	std::vector<int>::iterator it = std::find(edgeList->begin(), edgeList->end(), vb);
	if (it != edgeList->end()) {
		// if vb exists in va, remove it
		edgeList->erase(it);
	}
	/**
	 * Have to repeat for the opposite in case va=vb in which case calling this method twice
	 * would miss the second time.
	 */
	edgeList = getEdgeListForVertex(vb);
	it = std::find(edgeList->begin(), edgeList->end(), va);
	if (it != edgeList->end()){
		// if vb exists in va, remove it
		edgeList->erase(it);
	}
}

void UndirectedGraph::print() {

	printf(
			"numVertices: %ld, verticesA.length: %ld,  verticesB.length: %ld, edgeWeights.length: %ld, edges.length: %ld\n",
			numVertices, verticesA->size(), verticesB->size(), edgeWeights->size(),
			edges->size());
	printf("\nVertices A\n");

	for (vector<int>::iterator itr = verticesA->begin(); itr != verticesA->end(); itr++) {
		int vertex = *itr;
		printf("%d, ", vertex);
	}

	printf("\n\nVertices B\n");
	for (vector<int>::iterator itr = verticesB->begin(); itr != verticesB->end(); itr++) {
		int vertex = *itr;
		printf("%d, ", vertex);
	}

	printf("\n\nedgeWeights\n");
	for (vector<double>::iterator itr = edgeWeights->begin(); itr != edgeWeights->end(); itr++) {
		printf("%f, ", *itr);
	}

	printf("\n\nEdges\n");
	for (vector<vector<int> >::iterator itr = edges->begin(); itr != edges->end(); itr++) {
		//vector<int>* edge = itr;
		printf("[");
		for(vector<int>::iterator it = itr->begin(); it != itr->end(); ++it){
			printf("%d, ", *it);
		}
		printf("]\n");
	}
}

} /* namespace clustering */
