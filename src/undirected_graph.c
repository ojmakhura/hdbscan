/*
 * undirected_graph.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include "hdbscan/undirected_graph.h"

UndirectedGraph* graph_init(UndirectedGraph* g, int32_t numVertices, int32_t* verticesA,
		int32_t asize, int32_t* verticesB, int32_t bsize, double* edgeWeights, int32_t esize) {
	if(g == NULL)
		g = (UndirectedGraph*)malloc(sizeof(UndirectedGraph));

	if(g == NULL){
		printf("Error: Could not allocate memory for verticesA");
		return NULL;
	}

	g->asize = asize;
	g->bsize = bsize;
	g->esize = esize;

	/*printf("********************************************************************************\n");
	for(int i = 0; i < asize; i++){
		printf("%d \n", verticesA[i]);
	}
	printf("********************************************************************************\n");*/

	g->numVertices = numVertices;
	g->verticesA = verticesA;
	g->verticesB = verticesB;
	g->edgeWeights = edgeWeights;
	g->edges = NULL;
	g->edges = (IntList**) malloc(numVertices * sizeof(IntList*));

	if(g->edges == NULL){
		printf("Graph Init: Could not allocate memory for edges.\n");
		return NULL;
	}

	for(int32_t i = 0; i < numVertices; i++){
		IntList* list = NULL;
		g->edges[i] = list;
	}

	//graph_print(g);
	for (unsigned int i = 0; i < esize; i++) {

		int vertexOne = (g->verticesA)[i];
		int vertexTwo = (g->verticesB)[i];

		g->edges[vertexOne] = list_int_insert(g->edges[vertexOne], vertexTwo);
		//printf("vertexTwo = %d, vertexOne = %d\n", vertexTwo, vertexOne);
		if (vertexOne != vertexTwo) {
			g->edges[vertexTwo] = list_int_insert(g->edges[vertexTwo], vertexOne);
		}

	}

	//graph_print(g);

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
			free(g->verticesA);
		}

		if (g->verticesB != NULL) {
			free(g->verticesB);
		}

		if (g->edgeWeights != NULL) {
			free(g->edgeWeights);
		}

		if (g->edges != NULL) {

			for (int32_t i = 0; i < g->numVertices; i++) {
				IntList* list = g->edges[i];
				list_int_clean(list);
			}
			free(g->edges);
		}
	}
}

/**
 * Quicksorts the graph by edge weight in descending order. This quicksort implementation is
 * iterative and in-place.
 */
void graph_quicksort_by_edge_weight(UndirectedGraph* g) {
	if (g->esize <= 1)
		return;

	int startIndexStack[g->esize/2];
	int endIndexStack[g->esize/2];

	(startIndexStack)[0] = 0;
	(endIndexStack)[0] = g->esize - 1;
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

	//free(startIndexStack);
	//free(endIndexStack);
}

int32_t graph_select_pivot_index(UndirectedGraph* g, int32_t startIndex, int32_t endIndex) {
	if (startIndex - endIndex <= 1)
		return startIndex;

	double first = g->edgeWeights[startIndex];
	double middle = g->edgeWeights[startIndex + (endIndex - startIndex) / 2];
	double last = g->edgeWeights[endIndex];

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
		int32_t tempVertexA = g->verticesA[indexOne];
		int32_t tempVertexB = g->verticesB[indexOne];
		double tempEdgeDistance = g->edgeWeights[indexOne];

		g->verticesA[indexOne] = g->verticesA[indexTwo];
		g->verticesB[indexOne] = g->verticesB[indexTwo];
		g->edgeWeights[indexOne] = g->edgeWeights[indexTwo];

		g->verticesA[indexTwo] = tempVertexA;
		g->verticesB[indexTwo] = tempVertexB;
		g->edgeWeights[indexTwo] = tempEdgeDistance;
	}
}

int32_t graph_partition(UndirectedGraph* g, int32_t startIndex, int32_t endIndex, int32_t pivotIndex) {
	double pivotValue = g->edgeWeights[pivotIndex];
	graph_swap_edges(g, pivotIndex, endIndex);
	int32_t lowIndex = startIndex;

	for (int32_t i = startIndex; i < endIndex; i++) {
		if (g->edgeWeights[i] < pivotValue) {
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

IntList* do_remove(IntList* list, int32_t data){
	ListNode* node = g_list_find_custom(list, &data, (GCompareFunc)gint_compare);
	if(node != NULL){
		list = list_full_link_delete(list, node, (GDestroyNotify)free);
	}

	return list;
}

void graph_remove_edge(UndirectedGraph* g, int32_t va, int32_t vb){
	// get the edge list for va
	// find position for vb in the edge list for va

	g->edges[va] = do_remove(g->edges[va], vb);

	/**
	 * Have to repeat for the opposite in case va=vb in which case calling this method twice
	 * would miss the second time.
	 */
	//printf("in graph_remove_edge removing %d from edge %d\n", va, vb);
	g->edges[vb] = do_remove(g->edges[vb], va);

}

void graph_print(UndirectedGraph* g) {

	printf(
			"numVertices: %d, verticesA.length: %d,  verticesB.length: %d, edgeWeights.length: %d, edges.length: %d\n",
			g->numVertices, g->asize, g->bsize, g->esize, g->numVertices);
	printf("\nVertices A\n");

	for (int i = 0; i < g->asize; i++) {
		int vertex = g->verticesA[i];
		printf("%d, ", vertex);
	}

	printf("\n\nVertices B\n");
	for (int i = 0; i < g->bsize; i++) {
		int vertex = g->verticesB[i];
		printf("%d, ", vertex);
	}

	printf("\n\nedgeWeights\n");
    for (int i = 0; i < g->esize; i++) {
		printf("%f, ", g->edgeWeights[i]);
	}

	printf("\n\nEdges\n");

	for(uint i = 0; i < g->numVertices; i++){
		IntList* edge = g->edges[i];

		//printf("Edge length is %d\n", g_list_length(edge));
		printf("[");

		ListNode* node = g_list_first(edge);

		while(node != NULL){
			if(node->data != NULL){
				printf("%d, ", *((int32_t *)node->data));
			}

			node = g_list_next(node);
		}

		printf("]\n");
	}

}

