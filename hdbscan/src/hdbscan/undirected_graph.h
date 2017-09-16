/*
 * undirected_graph.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef UNDIRECTED_GRAPH_H_
#define UNDIRECTED_GRAPH_H_

#include <glib.h>

#define GRAPH_SUCCESS 1
#define GRAPH_ERROR 0

/**
 * An undirected graph, with weights assigned to each edge.  Vertices in the graph are 0 indexed.
 * @author junior
 */
typedef struct _UndirectedGraph {

	int numVertices;
	int* verticesA;
	int* verticesB;
	double* edgeWeights;
	int asize, bsize, esize;
	GList* edges = NULL;   				// This is a list of lists
} UndirectedGraph;

/**
 * Quicksorts the graph in the interval [startIndex, endIndex] by edge weight.
 * @param startIndex The lowest index to be included in the sort
 * @param endIndex The highest index to be included in the sort
 */
void graph_quicksort(UndirectedGraph* g, int startIndex, int endIndex);

/**
 * Returns a pivot index by finding the median of edge weights between the startIndex, endIndex,
 * and middle.
 * @param startIndex The lowest index from which the pivot index should come
 * @param endIndex The highest index from which the pivot index should come
 * @return A pivot index
 */
int graph_select_pivot_index(UndirectedGraph* g, int startIndex, int endIndex);

/**
 * Partitions the array in the interval [startIndex, endIndex] around the value at pivotIndex.
 * @param startIndex The lowest index to  partition
 * @param endIndex The highest index to partition
 * @param pivotIndex The index of the edge weight to partition around
 * @return The index position of the pivot edge weight after the partition
 */
int graph_partition(UndirectedGraph* g, int startIndex, int endIndex, int pivotIndex);

/**
 * Swaps the vertices and edge weights between two index locations in the graph.
 * @param indexOne The first index location
 * @param indexTwo The second index location
 */
void graph_swap_edges(UndirectedGraph* g, int indexOne, int indexTwo);

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
int graph_init(UndirectedGraph* g, int numVertices, int* verticesA, int asize, int* verticesB, int bsize, double* edgeWeights, int esize) ;

void graph_destroy(UndirectedGraph* g);

/**
 * Quicksorts the graph by edge weight in descending order.  This quicksort implementation is
 * iterative and in-place.
 */
void graph_quicksort_by_edge_weight(UndirectedGraph* g);

// ------------------------------ GETTERS & SETTERS ------------------------------

int graph_get_num_vertices();

int graph_get_num_edges();

int graph_get_first_vertex_at_index(UndirectedGraph* g, int index);

int graph_get_second_vertex_at_index(UndirectedGraph* g, int index);

double graph_get_edge_weight_at_index(UndirectedGraph* g, int index);

int* graph_get_edge_list_for_vertex(UndirectedGraph* g, int vertex);
/**
 * Remove vb from edge list of va
 */
void graph_remove_edge(UndirectedGraph* g, int va, int vb);

void graph_print(UndirectedGraph* g);

#endif /* UNDIRECTED_GRAPH_H_ */
