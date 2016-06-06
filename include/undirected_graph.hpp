/*
 * undirected_graph.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef UNDIRECTED_GRAPH_H_
#define UNDIRECTED_GRAPH_H_

#include <vector>
#include <cstddef>

using namespace std;
namespace clustering {

/**
 * An undirected graph, with weights assigned to each edge.  Vertices in the graph are 0 indexed.
 * @author junior
 */
class UndirectedGraph {

private:
	int numVertices;
	vector<int>* verticesA;
	vector<int>* verticesB;
	vector<double>* edgeWeights;
	vector<vector<int> > *edges;

	/**
	 * Quicksorts the graph in the interval [startIndex, endIndex] by edge weight.
	 * @param startIndex The lowest index to be included in the sort
	 * @param endIndex The highest index to be included in the sort
	 */
	void quicksort(int startIndex, int endIndex);

	/**
	 * Returns a pivot index by finding the median of edge weights between the startIndex, endIndex,
	 * and middle.
	 * @param startIndex The lowest index from which the pivot index should come
	 * @param endIndex The highest index from which the pivot index should come
	 * @return A pivot index
	 */
	int selectPivotIndex(int startIndex, int endIndex);

	/**
	 * Partitions the array in the interval [startIndex, endIndex] around the value at pivotIndex.
	 * @param startIndex The lowest index to  partition
	 * @param endIndex The highest index to partition
	 * @param pivotIndex The index of the edge weight to partition around
	 * @return The index position of the pivot edge weight after the partition
	 */
	int partition(int startIndex, int endIndex, int pivotIndex);

	/**
	 * Swaps the vertices and edge weights between two index locations in the graph.
	 * @param indexOne The first index location
	 * @param indexTwo The second index location
	 */
	void swapEdges(int indexOne, int indexTwo);

public:
	virtual ~UndirectedGraph();
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
	UndirectedGraph(int numVertices, vector<int>* verticesA,
			vector<int>* verticesB, vector<double>* edgeWeights);

	// ------------------------------ PUBLIC METHODS ------------------------------

	/**
	 * Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
	 * iterative and in-place.
	 */
	void quicksortByEdgeWeight();

	// ------------------------------ PRIVATE METHODS ------------------------------

	// ------------------------------ GETTERS & SETTERS ------------------------------

	int getNumVertices();

	int getNumEdges();

	int getFirstVertexAtIndex(int index);

	int getSecondVertexAtIndex(int index);

	double getEdgeWeightAtIndex(int index);

	vector<int>* getEdgeListForVertex(int vertex);
	/**
	 * Remove vb from edge list of va
	 */
	void removeEdge(int va, int vb);

	void print();
};

} /* namespace clustering */

#endif /* UNDIRECTED_GRAPH_H_ */
