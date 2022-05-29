/*
 * undirected_graph.h
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

/** @file undirected_graph.h */
#ifndef UNDIRECTED_GRAPH_H_
#define UNDIRECTED_GRAPH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "hdbscan/utils.h"
#include "collections/list.h"

#define GRAPH_SUCCESS 1
#define GRAPH_ERROR 0
// This is an array of lists
#ifdef __cplusplus
namespace clustering {
#endif
/**
 * \struct UndirectedGraph 
 * \brief An undirected graph, with weights assigned to each edge.  Vertices in the graph are 0 indexed.
 */
struct UndirectedGraph {

	index_t numVertices;
	ArrayList* verticesA;
	ArrayList* verticesB;
	ArrayList* edgeWeights;
	ArrayList** edges;   				// This is an array of lists

#ifdef __cplusplus
public:
	virtual ~UndirectedGraph();
	// ------------------------------ CONSTRUCTORS ------------------------------

	/**
	 * 
	 * @param numVertices The number of vertices in the graph (indexed 0 to numVertices-1)
	 * @param verticesA An array of vertices corresponding to the array of edges
	 * @param verticesB An array of vertices corresponding to the array of edges
	 * @param edgeWeights An array of edges corresponding to the arrays of vertices
	 */

	/**
	 * @brief Constructs a new UndirectedGraph, including creating an edge list for each vertex from the
	 * vertex arrays.  For an index i, verticesA[i] and verticesB[i] share an edge with weight
	 * edgeWeights[i].
	 * 
	 * @param numVertices The number of vertices in the graph (indexed 0 to numVertices-1)
	 * @param verticesA An array of vertices corresponding to the array of edges
	 * @param verticesB An array of vertices corresponding to the array of edges
	 * @param edgeWeights An array of edges corresponding to the arrays of vertices
	 */
	UndirectedGraph(index_t numVertices, ArrayList* verticesA, ArrayList* verticesB, ArrayList* edgeWeights);

	/**
	 * @brief Construct a new Undirected Graph object
	 * 
	 */
	UndirectedGraph();

	// ------------------------------ PUBLIC METHODS ------------------------------

	/**
	 * @brief Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
	 * iterative and in-place.
	 * 
	 */
	void quicksortByEdgeWeight();

	/**
	 * @brief Remove vb from edge list of va
	 * 
	 * @param va 
	 * @param vb 
	 */
	void removeEdge(int va, int vb);

	/**
	 * @brief Print the graph
	 * 
	 */
	void print();

	/**
	 * @brief Clean the graph without deallocating its memory
	 * 
	 */
	void clean();

#endif
};

/** \typedef UndirectedGraph */
typedef struct UndirectedGraph UndirectedGraph;

/**
 * @brief Quicksorts the graph in the interval [startIndex, endIndex] by edge weight.
 * 
 * @param startIndex The lowest index to be included in the sort
 * @param endIndex The highest index to be included in the sort
 */
void graph_quicksort(UndirectedGraph* g, int64_t startIndex, int64_t endIndex);

/**
 * @brief Returns a pivot index by finding the median of edge weights between the startIndex, endIndex,
 * and middle.
 * 
 * @param startIndex The lowest index from which the pivot index should come
 * @param endIndex The highest index from which the pivot index should come
 * @return A pivot index
 */
int32_t graph_select_pivot_index(UndirectedGraph* g, int64_t startIndex, int64_t endIndex);

/**
 * @brief Partitions the array in the interval [startIndex, endIndex] around the value at pivotIndex.
 * 
 * @param startIndex The lowest index to  partition
 * @param endIndex The highest index to partition
 * @param pivotIndex The index of the edge weight to partition around
 * @return The index position of the pivot edge weight after the partition
 */
int32_t graph_partition(UndirectedGraph* g, int64_t startIndex, int64_t endIndex, int64_t pivotIndex);

/**
 * @brief Swaps the vertices and edge weights between two index locations in the graph.
 * 
 * @param indexOne The first index location
 * @param indexTwo The second index location
 */
void graph_swap_edges(UndirectedGraph* g, int64_t indexOne, int64_t indexTwo);

// ------------------------------ CONSTRUCTORS ------------------------------

/**
 * @brief 
 * 
 * 
 */

/**
 * @brief Constructs a new UndirectedGraph, including creating an edge list for each vertex from the
 * vertex arrays.  For an index i, verticesA[i] and verticesB[i] share an edge with weight
 * edgeWeights[i].
 * 
 * @param g 
 * @param numVertices The number of vertices in the graph (indexed 0 to numVertices-1)
 * @param verticesA An array of vertices corresponding to the array of edges
 * @param verticesB An array of vertices corresponding to the array of edges
 * @param edgeWeights An array of edges corresponding to the arrays of vertices
 * @return UndirectedGraph* 
 */
UndirectedGraph* graph_init(UndirectedGraph* g, index_t numVertices, ArrayList* verticesA, ArrayList* verticesB, ArrayList* edgeWeights) ;

/**
 * @brief Deallocate memory for UndirectedGraph components and for the graph itself
 * 
 * @param g 
 */
void graph_destroy(UndirectedGraph* g);

/**
 * @brief Deallocate memory for UndirectedGraph components
 * 
 * @param g 
 */
void graph_clean(UndirectedGraph* g);

/**
 * @brief Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
 * iterative and in-place.
 * 
 * @param g 
 */
void graph_quicksort_by_edge_weight(UndirectedGraph* g);

// ------------------------------ GETTERS & SETTERS ------------------------------

/**
 * @brief 
 * 
 * @return int32_t 
 */
int32_t graph_get_num_vertices();

/**
 * @brief 
 * 
 * @return int32_t 
 */
int32_t graph_get_num_edges();

/**
 * @brief 
 * 
 * @param g 
 * @param index 
 * @return int32_t 
 */
int32_t graph_get_first_vertex_at_index(UndirectedGraph* g, int32_t index);

/**
 * @brief 
 * 
 * @param g 
 * @param index 
 * @return int32_t 
 */
int32_t graph_get_second_vertex_at_index(UndirectedGraph* g, int32_t index);

/**
 * @brief 
 * 
 * @param g 
 * @param index 
 * @return double 
 */
distance_t graph_get_edge_weight_at_index(UndirectedGraph* g, int32_t index);

/**
 * @brief 
 * 
 * @param g 
 * @param vertex 
 * @return ArrayList* 
 */
ArrayList* graph_get_edge_list_for_vertex(UndirectedGraph* g, int32_t vertex);

/**
 * @brief Remove vb from edge list of va
 * 
 * @param g 
 * @param va 
 * @param vb 
 */
void graph_remove_edge(UndirectedGraph* g, index_t va, index_t vb);

/**
 * @brief 
 * 
 * @param g 
 */
void graph_print(UndirectedGraph* g);

#ifdef __cplusplus
};
}
#endif

#endif /* UNDIRECTED_GRAPH_H_ */
