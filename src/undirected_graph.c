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
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "hdbscan/undirected_graph.h"
//#include <omp.h>

UndirectedGraph* graph_init(UndirectedGraph* g, int32_t numVertices, IntArrayList* verticesA, IntArrayList* verticesB, DoubleArrayList* edgeWeights) {
	if(g == NULL)
		g = (UndirectedGraph*)malloc(sizeof(UndirectedGraph));

	if(g == NULL){
		printf("Error: Could not allocate memory for graph");
		return NULL;
	}

	g->numVertices = numVertices;
	g->verticesA = verticesA;
	g->verticesB = verticesB;
	g->edgeWeights = edgeWeights;
	g->edges = (IntArrayList**) malloc(numVertices * sizeof(IntArrayList*));

	if(g->edges == NULL){
		printf("Graph Init: Could not allocate memory for edges.\n");
		return NULL;
	}

//#pragma omp parallel for
	for(int32_t i = 0; i < numVertices; i++){
		g->edges[i] = int_array_list_init();
	}

	for (unsigned int i = 0; i < g->verticesA->size; i++) {

		int vertexOne = *(int_array_list_data(g->verticesA, i));
		int vertexTwo = *(int_array_list_data(g->verticesB, i));

		int_array_list_append(g->edges[vertexOne], vertexTwo);
		if (vertexOne != vertexTwo) {
			int_array_list_append(g->edges[vertexTwo], vertexOne);
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
			int_array_list_delete(g->verticesA);
			g->verticesA = NULL;
		}

		if (g->verticesB != NULL) {
			int_array_list_delete(g->verticesB);
			g->verticesB = NULL;
		}

		if (g->edgeWeights != NULL) {
			double_array_list_delete(g->edgeWeights);
			g->edgeWeights = NULL;
		}

		if (g->edges != NULL) {

			for (int32_t i = 0; i < g->numVertices; i++) {
				IntArrayList* list = g->edges[i];
				int_array_list_delete(list);
			}
			free(g->edges);
			g->edges = NULL;
		}
	}
}

void graph_quicksort_by_edge_weight(UndirectedGraph* g) {
	int32_t esize = g->edgeWeights->size;
	if (esize <= 1)
		return;

	int startIndexStack[esize/2];
	int endIndexStack[esize/2];

	(startIndexStack)[0] = 0;
	(endIndexStack)[0] = esize - 1;
	int stackTop = 0;

	while (stackTop >= 0) {
		int32_t startIndex = (startIndexStack)[stackTop];
		int32_t endIndex = (endIndexStack)[stackTop];
		stackTop--;

		int pivotIndex = graph_select_pivot_index(g, startIndex, endIndex);
		pivotIndex = graph_partition(g, startIndex, endIndex, pivotIndex);

		if (pivotIndex > startIndex + 1) {
			(startIndexStack)[stackTop + 1] = startIndex;
			(endIndexStack)[stackTop + 1] = pivotIndex - 1;
			stackTop++;
		}

		if (pivotIndex < endIndex - 1) {
			(startIndexStack)[stackTop + 1] = pivotIndex + 1;
			(endIndexStack)[stackTop + 1] = endIndex;
			stackTop++;
		}
	}
}

int32_t graph_select_pivot_index(UndirectedGraph* g, int32_t startIndex, int32_t endIndex) {
	if (startIndex - endIndex <= 1)
		return startIndex;

	double first = *(double_array_list_data(g->edgeWeights, startIndex));
	double middle = *(double_array_list_data(g->edgeWeights, startIndex + (endIndex - startIndex) / 2));
	double last = *(double_array_list_data(g->edgeWeights, endIndex));

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

void graph_swap_edges(UndirectedGraph* g, int32_t indexOne, int32_t indexTwo){
	if (indexOne != indexTwo){
		int32_t tempVertexA = *(int_array_list_data(g->verticesA, indexOne));
		int32_t tempVertexB = *(int_array_list_data(g->verticesB, indexOne));
		double tempEdgeDistance = *(double_array_list_data(g->edgeWeights, indexOne));

		int_array_list_set_value_at(g->verticesA, *(int_array_list_data(g->verticesA, indexTwo)), indexOne);
		int_array_list_set_value_at(g->verticesB, *(int_array_list_data(g->verticesB, indexTwo)), indexOne);
		double_array_list_set_value_at(g->edgeWeights, *(double_array_list_data(g->edgeWeights, indexTwo)), indexOne);

		int_array_list_set_value_at(g->verticesA, tempVertexA, indexTwo);
		int_array_list_set_value_at(g->verticesB, tempVertexB, indexTwo);
		double_array_list_set_value_at(g->edgeWeights, tempEdgeDistance, indexTwo);
	}
}

int32_t graph_partition(UndirectedGraph* g, int32_t startIndex, int32_t endIndex, int32_t pivotIndex) {
	double pivotValue = *(double_array_list_data(g->edgeWeights, pivotIndex));//g->edgeWeights[pivotIndex];
	graph_swap_edges(g, pivotIndex, endIndex);
	int32_t lowIndex = startIndex;

	for (int32_t i = startIndex; i < endIndex; i++) {
		double c = *(double_array_list_data(g->edgeWeights, i));
		if (c < pivotValue) {
			graph_swap_edges(g, i, lowIndex);
			lowIndex++;
		}
	}

	graph_swap_edges(g, lowIndex, endIndex);
	return lowIndex;
}

void graph_quicksort(UndirectedGraph* g, int32_t startIndex, int32_t endIndex) {
	if (startIndex < endIndex) {
		int32_t pivotIndex = graph_select_pivot_index(g, startIndex, endIndex);
		pivotIndex = graph_partition(g, startIndex, endIndex, pivotIndex);
		graph_quicksort(g, startIndex, pivotIndex - 1);
		graph_quicksort(g, pivotIndex + 1, endIndex);
	}
}

void graph_remove_edge(UndirectedGraph* g, int32_t va, int32_t vb){
	// get the edge list for va
	// find position for vb in the edge list for va

	int_array_list_remove(g->edges[va], vb);

	/**
	 * Have to repeat for the opposite in case va=vb in which case calling this method twice
	 * would miss the second time.
	 */
	int_array_list_remove(g->edges[vb], va);

}

void graph_print(UndirectedGraph* g) {

	printf(
			"numVertices: %d, verticesA.length: %d,  verticesB.length: %d, edgeWeights.length: %d, edges.length: %d\n",
			g->numVertices, g->verticesA->size, g->verticesB->size, g->edgeWeights->size, g->numVertices);
	printf("\nVertices A\n");

	for (int i = 0; i < g->verticesA->size; i++) {
		int vertex = *(int_array_list_data(g->verticesA, i));
		printf("%d, ", vertex);
	}

	printf("\n\nVertices B\n");
	for (int i = 0; i < g->verticesB->size; i++) {
		int vertex = *(int_array_list_data(g->verticesB, i));
		printf("%d, ", vertex);
	}

	printf("\n\nedgeWeights\n");
    for (int i = 0; i < g->edgeWeights->size; i++) {
		printf("%f, ", *(double_array_list_data(g->edgeWeights, i)));
	}

	printf("\n\nEdges\n");

	for(uint i = 0; i < g->numVertices; i++){
		IntArrayList* edge = g->edges[i];

		//printf("Edge length is %d\n", g_list_length(edge));
		printf("[");
		int32_t* ldata = edge->data;

		for(int32_t i = 0; i < edge->size; i++){
			printf("%d, ", ldata[i]);
		}

		printf("]\n");
	}

}

