/*
 * undirected_graph.c
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

/**
 * @file undirected_graph.c
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief UndirectedGraph implementation.
 * 
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "hdbscan/undirected_graph.h"
#include "hdbscan/logger.h"

#ifdef _OPENMP
#include <omp.h>
#endif

UndirectedGraph* graph_init(UndirectedGraph* g, index_t numVertices, ArrayList* verticesA, ArrayList* verticesB, ArrayList* edgeWeights) {
	if(g == NULL)
		g = (UndirectedGraph*)malloc(sizeof(UndirectedGraph));

	if(g == NULL){
	#ifdef DEBUG
		logger_write(FATAL, "graph_init - Could not allocate memory for graph.");
	#else
		printf("FATAL: graph_init - Could not allocate memory for graph.");
	#endif
		return NULL;
	}

	g->numVertices = numVertices;
	g->verticesA = verticesA;
	g->verticesB = verticesB;
	g->edgeWeights = edgeWeights;
	g->edges = (ArrayList**) malloc(numVertices * sizeof(ArrayList*));

	if(g->edges == NULL){

	#ifdef DEBUG
		logger_write(FATAL, "graph_init - Could not allocate memory for edges.");
	#else
		printf("FATAL: graph_init - Could not allocate memory for edges.");
	#endif
		return NULL;
	}

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for(index_t i = 0; i < numVertices; i++){
		g->edges[i] = array_list_init(16, sizeof(index_t), NULL);
		if(sizeof(index_t) == sizeof(int)) {
			g->edges[i]->compare = int_compare;
		} else if(sizeof(index_t) == sizeof(long)) {
			g->edges[i]->compare = long_compare;
		} else {
			g->edges[i]->compare = short_compare;
		}
	}
	index_t vertexOne, vertexTwo;
	for (index_t i = 0; i < g->verticesA->size; i++) {
		
		array_list_value_at(g->verticesA, i, &vertexOne);
		array_list_value_at(g->verticesB, i, &vertexTwo);

		array_list_append(g->edges[vertexOne], &vertexTwo);
		if (vertexOne != vertexTwo) {
			array_list_append(g->edges[vertexTwo], &vertexOne);
		}
	}

	return g;
}

void graph_destroy(UndirectedGraph* g) {
	if(g != NULL){
		graph_clean(g);
		free(g);
	}

}
void graph_clean(UndirectedGraph* g) {
	if(g != NULL){

		if (g->verticesA != NULL) {
			array_list_delete(g->verticesA);
			g->verticesA = NULL;
		}

		if (g->verticesB != NULL) {
			array_list_delete(g->verticesB);
			g->verticesB = NULL;
		}

		if (g->edgeWeights != NULL) {
			array_list_delete(g->edgeWeights);
			g->edgeWeights = NULL;
		}

		if (g->edges != NULL) {

			for (index_t i = 0; i < g->numVertices; i++) {
				ArrayList* list = g->edges[i];
				array_list_delete(list);
			}
			free(g->edges);
			g->edges = NULL;
		}
	}
}

/**
 * @brief An edge copied out of the graph's parallel arrays for sorting
 */
typedef struct {
	distance_t weight;
	index_t a, b;
} graph_sort_edge;

static int graph_sort_edge_compare(const void* x, const void* y) {
	const graph_sort_edge* e1 = (const graph_sort_edge*)x;
	const graph_sort_edge* e2 = (const graph_sort_edge*)y;

	if (e1->weight != e2->weight)
		return e1->weight < e2->weight ? -1 : 1;

	// Break ties on the vertices so the order does not depend on the qsort implementation
	if (e1->a != e2->a)
		return e1->a < e2->a ? -1 : 1;

	if (e1->b != e2->b)
		return e1->b < e2->b ? -1 : 1;

	return 0;
}

void graph_quicksort_by_edge_weight(UndirectedGraph* g) {
	size_t esize = g->edgeWeights->size;
	if (esize <= 1)
		return;

	distance_t* dt = g->edgeWeights->data;
	index_t* da = g->verticesA->data;
	index_t* db = g->verticesB->data;

	/// Sort a packed copy of the edges: qsort keeps this O(n log n) even with many equal weights
	graph_sort_edge* edges = (graph_sort_edge*)malloc(esize * sizeof(graph_sort_edge));
	if (edges == NULL) {
		logger_write(FATAL, "graph_quicksort_by_edge_weight - Could not allocate memory for sorting.");
		graph_quicksort(g, 0, (int64_t)esize - 1);
		return;
	}

	for (size_t i = 0; i < esize; i++) {
		edges[i].weight = dt[i];
		edges[i].a = da[i];
		edges[i].b = db[i];
	}

	qsort(edges, esize, sizeof(graph_sort_edge), graph_sort_edge_compare);

	for (size_t i = 0; i < esize; i++) {
		dt[i] = edges[i].weight;
		da[i] = edges[i].a;
		db[i] = edges[i].b;
	}

	free(edges);
}

int32_t graph_select_pivot_index(UndirectedGraph* g, int64_t startIndex, int64_t endIndex) {
	if (endIndex - startIndex <= 1)
		return (int32_t)startIndex;

	distance_t first, middle, last;
	array_list_value_at(g->edgeWeights, (size_t)startIndex, &first);
	array_list_value_at(g->edgeWeights, (size_t)(startIndex + (endIndex - startIndex) / 2), &middle);
	array_list_value_at(g->edgeWeights, (size_t)endIndex, &last);

	if (first <= middle) {
		if (middle <= last)
			return  (int32_t)(startIndex + (endIndex - startIndex) / 2);
		else if (last >= first)
			return (int32_t)endIndex;
		else
			return (int32_t)startIndex;
	} else {
		if (first <= last)
			return (int32_t)startIndex;
		else if (last >= middle)
			return (int32_t)endIndex;
		else
			return (int32_t)(startIndex + (endIndex - startIndex) / 2);
	}
}

void graph_swap_edges(UndirectedGraph* g, int64_t indexOne, int64_t indexTwo){
	if (indexOne != indexTwo){
		distance_t* dt = g->edgeWeights->data;
		index_t* da = g->verticesA->data;
		index_t* db = g->verticesB->data;

		index_t tempVertexA, tempVertexB;
		tempVertexA = da[indexOne];
		tempVertexB = db[indexOne];
		
		distance_t tempEdgeDistance;
		tempEdgeDistance = dt[indexOne];

		da[indexOne] = da[indexTwo];
		db[indexOne] = db[indexTwo];
		dt[indexOne] = dt[indexTwo];

		da[indexTwo] = tempVertexA;
		db[indexTwo] = tempVertexB;
		dt[indexTwo] = tempEdgeDistance;

	}
}

int32_t graph_partition(UndirectedGraph* g, int64_t startIndex, int64_t endIndex, int64_t pivotIndex) {
	distance_t pivotValue;
	distance_t* dt = g->edgeWeights->data;
	pivotValue = dt[pivotIndex];
	graph_swap_edges(g, pivotIndex, endIndex);
	int64_t lowIndex = startIndex;
	distance_t c;

	for (int64_t i = startIndex; i < endIndex; i++) {
		c = dt[i];
		if (c < pivotValue) {
			graph_swap_edges(g, i, lowIndex);
			lowIndex++;
		}
	}

	graph_swap_edges(g, lowIndex, endIndex);
	return (int32_t)lowIndex;
}

void graph_quicksort(UndirectedGraph* g, int64_t startIndex, int64_t endIndex) {
	if (startIndex < endIndex) {
		int64_t pivotIndex = graph_select_pivot_index(g, startIndex, endIndex);
		pivotIndex = graph_partition(g, startIndex, endIndex, pivotIndex);
		graph_quicksort(g, startIndex, pivotIndex - 1);
		graph_quicksort(g, pivotIndex + 1, endIndex);
	}
}

void graph_remove_edge(UndirectedGraph* g, index_t va, index_t vb){
	// get the edge list for va
	// find position for vb in the edge list for va
	array_list_remove(g->edges[va], &vb);
	
	/**
	 * Have to repeat for the opposite in case va=vb in which case calling this method twice
	 * would miss the second time.
	 */
	array_list_remove(g->edges[vb], &va);

}

void graph_print(UndirectedGraph* g) {
	distance_t* dt = g->edgeWeights->data;
	index_t* da = g->verticesA->data;
	index_t* db = g->verticesB->data;

	char s[300];
	snprintf(s, sizeof(s),
			"numVertices: %d, verticesA.length: %ld,  verticesB.length: %ld, edgeWeights.length: %ld, edges.length: %d\n",
			g->numVertices, g->verticesA->size, g->verticesB->size, g->edgeWeights->size, g->numVertices);
	snprintf(s + strlen(s), sizeof(s) - strlen(s), "\nVertices A\n");

	logger_write(NONE, s);

	for (size_t i = 0; i < g->verticesA->size; i++) {
		index_t vertex = da[i];
	#ifdef DEBUG
		snprintf(s, sizeof(s), "%d, ", vertex);
		logger_write(NONE, s);
	#else
		printf("%d, ", vertex);
	#endif
		
	}

	snprintf(s, sizeof(s), "\n\nVertices B\n");
	logger_write(NONE, s);

	for (size_t i = 0; i < g->verticesB->size; i++) {
		index_t vertex = db[i];
	
		snprintf(s, sizeof(s), "%d, ", vertex);
		logger_write(NONE, s);
	}

	snprintf(s, sizeof(s), "\n\nnedgeWeights\n");
	logger_write(NONE, s);

    for (size_t i = 0; i < g->edgeWeights->size; i++) {
		distance_t weight = dt[i];
		snprintf(s, sizeof(s), "%f, ", weight);
		logger_write(NONE, s);
	}

	snprintf(s, sizeof(s), "\n\nEdges\n");
	logger_write(NONE, s);

	for(uint i = 0; i < g->numVertices; i++){
		ArrayList* edge = g->edges[i];
		logger_write(NONE, "[");
	
		index_t* ldata = edge->data;

		for(index_t i = 0; i < edge->size; i++){
			snprintf(s, sizeof(s), "%d, ", ldata[i]);
			logger_write(NONE, s);			
		}
	
		logger_write(NONE, "]\n");
	}

}

