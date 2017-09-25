/*
 * undirected_graph.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef UNDIRECTED_GRAPH_H_
#define UNDIRECTED_GRAPH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include "utils.h"

#define GRAPH_SUCCESS 1
#define GRAPH_ERROR 0
// This is an array of lists
#ifdef __cplusplus
namespace clustering {
#endif
/**
 * An undirected graph, with weights assigned to each edge.  Vertices in the graph are 0 indexed.
 * @author junior
 */
struct UndirectedGraph {

	int32_t numVertices;
	int32_t* verticesA;
	int32_t* verticesB;
	double* edgeWeights;
	int32_t asize, bsize, esize;
	IntList** edges;   				// This is an array of lists

#ifdef __cplusplus
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
	UndirectedGraph(int32_t numVertices, int32_t* verticesA, int32_t asize, int32_t* verticesB, int32_t bsize, double* edgeWeights, int32_t esize);
	UndirectedGraph();

	// ------------------------------ PUBLIC METHODS ------------------------------

	/**
	 * Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
	 * iterative and in-place.
	 */
	void quicksortByEdgeWeight();
	/**
	 * Remove vb from edge list of va
	 */
	void removeEdge(int va, int vb);

	void print();

	void clean();

#endif
};

typedef struct UndirectedGraph UndirectedGraph;

/**
 * Quicksorts the graph in the interval [startIndex, endIndex] by edge weight.
 * @param startIndex The lowest index to be included in the sort
 * @param endIndex The highest index to be included in the sort
 */
void graph_quicksort(UndirectedGraph* g, int32_t startIndex, int32_t endIndex);

/**
 * Returns a pivot index by finding the median of edge weights between the startIndex, endIndex,
 * and middle.
 * @param startIndex The lowest index from which the pivot index should come
 * @param endIndex The highest index from which the pivot index should come
 * @return A pivot index
 */
int32_t graph_select_pivot_index(UndirectedGraph* g, int32_t startIndex, int32_t endIndex);

/**
 * Partitions the array in the interval [startIndex, endIndex] around the value at pivotIndex.
 * @param startIndex The lowest index to  partition
 * @param endIndex The highest index to partition
 * @param pivotIndex The index of the edge weight to partition around
 * @return The index position of the pivot edge weight after the partition
 */
int32_t graph_partition(UndirectedGraph* g, int32_t startIndex, int32_t endIndex, int32_t pivotIndex);

/**
 * Swaps the vertices and edge weights between two index locations in the graph.
 * @param indexOne The first index location
 * @param indexTwo The second index location
 */
void graph_swap_edges(UndirectedGraph* g, int32_t indexOne, int32_t indexTwo);

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
UndirectedGraph* graph_init(UndirectedGraph* g, int32_t numVertices, int32_t* verticesA, int32_t asize, int32_t* verticesB, int32_t bsize, double* edgeWeights, int32_t esize) ;

/**
 * Deallocate memory for UndirectedGraph components and for the graph itself
 */
void graph_destroy(UndirectedGraph* g);

/**
 * Deallocate memory for UndirectedGraph components
 */
void graph_clean(UndirectedGraph* g);

/**
 * Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
 * iterative and in-place.
 */
void graph_quicksort_by_edge_weight(UndirectedGraph* g);

// ------------------------------ GETTERS & SETTERS ------------------------------

int32_t graph_get_num_vertices();

int32_t graph_get_num_edges();

int32_t graph_get_first_vertex_at_index(UndirectedGraph* g, int32_t index);

int32_t graph_get_second_vertex_at_index(UndirectedGraph* g, int32_t index);

double graph_get_edge_weight_at_index(UndirectedGraph* g, int32_t index);

IntList* graph_get_edge_list_for_vertex(UndirectedGraph* g, int32_t vertex);
/**
 * Remove vb from edge list of va
 */
void graph_remove_edge(UndirectedGraph* g, int32_t va, int32_t vb);

void graph_print(UndirectedGraph* g);

#ifdef __cplusplus
};
}
#endif

#endif /* UNDIRECTED_GRAPH_H_ */
