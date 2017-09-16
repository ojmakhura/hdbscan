/*
 * undirected_graph.c
 *
 *  Created on: 18 Apr 2017
 *      Author: ojmakh
 */
#include "hdbscan/undirected_graph.h"

int graph_init(UndirectedGraph* g, int numVertices, int* verticesA,
		int asize, int* verticesB, int bsize, double* edgeWeights, int esize) {

	g = (UndirectedGraph*)malloc(sizeof(UndirectedGraph));

	if(g == NULL){
		printf("Error: Could not allocate memory for verticesA");
		return GRAPH_ERROR;
	}

	g->asize = asize;
	g->bsize = bsize;
	g->esize = esize;

	g->numVertices = numVertices;
	g->verticesA = verticesA;
	g->verticesB = verticesB;
	g->edgeWeights = edgeWeights;

	for(int i = 0; i < numVertices; i++){
		g->edges = g_list_append(g->edges, NULL);
	}

	for (unsigned int i = 0; i < esize(); i++) {

		int vertexOne = (g->verticesA)[i];
		int vertexTwo = (g->verticesB)[i];
		int* data = (int *) malloc(sizeof(int));

		if(data == NULL){
			printf("Error: Could not allocate memory for data");
			return GRAPH_ERROR;
		}

		*data = vertexTwo;
		GList* list = g_list_nth_data(g->edges, vertexOne);
		list = g_list_append(list, data);

		if (vertexOne != vertexTwo) {
			data = (int *) malloc(sizeof(int));
			*data = vertexOne;
			list = g_list_nth_data(g->edges, vertexTwo);
		}
	}

	return GRAPH_SUCCESS;
}

void graph_destroy(UndirectedGraph* g) {

	if(g->verticesA != NULL){
		free(g->verticesA);
	}

	if(g->verticesB != NULL){
		free(g->verticesB);
	}

	if(g->edgeWeights != NULL){
		free(g->edgeWeights);
	}

	int size = g_list_length(g->edges);

	for(int i = 0; i < size; i++){
		GList* list = g_list_nth_data(g->edges, i);
		g_list_free_full(list, (GDestroyNotify)free);
	}

	if(size > 0){
		g_list_free(g->edges);
	}

	free(g);
}

/**
 * Quicksorts the graph by edge weight in descending order. This quicksort implementation is
 * iterative and in-place.
 */
void graph_quicksort_by_edge_weight(UndirectedGraph* g) {
	if (g->esize <= 1)
		return;

	int* startIndexStack = (g->esize/2) * malloc(sizeof(int));
	int* endIndexStack = (g->esize/2) * malloc(sizeof(int));

	(startIndexStack)[0] = 0;
	(endIndexStack)[0] = g->esize() - 1;
	int stackTop = 0;

	while (stackTop >= 0) {
		int startIndex = (startIndexStack)[stackTop];
		int endIndex = (endIndexStack)[stackTop];
		stackTop--;

		int pivotIndex = selectPivotIndex(startIndex, endIndex);
		pivotIndex = partition(startIndex, endIndex, pivotIndex);

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

	free(startIndexStack);
	free(endIndexStack);
}

int graph_select_pivot_index(UndirectedGraph* g, int startIndex, int endIndex) {
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

void graph_swap_edges(UndirectedGraph* g, int indexOne, int indexTwo){
	if (indexOne != indexTwo){
		int tempVertexA = g->verticesA[indexOne];
		int tempVertexB = g->verticesB[indexOne];
		double tempEdgeDistance = g->edgeWeights[indexOne];

		g->verticesA[indexOne] = g->verticesA[indexTwo];
		g->verticesB[indexOne] = g->verticesB[indexTwo];
		g->edgeWeights[indexOne] = g->edgeWeights[indexTwo];

		g->verticesA[indexTwo] = tempVertexA;
		g->verticesB[indexTwo] = tempVertexB;
		g->edgeWeights[indexTwo] = tempEdgeDistance;
	}
}

int graph_partition(UndirectedGraph* g, int startIndex, int endIndex, int pivotIndex) {
	double pivotValue = g->edgeWeights[pivotIndex];
	graph_swap_edges(pivotIndex, endIndex);
	int lowIndex = startIndex;

	for (int i = startIndex; i < endIndex; i++) {
		if (g->edgeWeights[i] < pivotValue) {
			graph_swap_edges(i, lowIndex);
			lowIndex++;
		}
	}

	graph_swap_edges(lowIndex, endIndex);
	return lowIndex;
}

void graph_quicksort(UndirectedGraph* g, int startIndex, int endIndex) {
	if (startIndex < endIndex) {
		int pivotIndex = selectPivotIndex(g, startIndex, endIndex);
		pivotIndex = partition(startIndex, endIndex, pivotIndex);
		graph_quicksort(g, startIndex, pivotIndex - 1);
		graph_quicksort(g, pivotIndex + 1, endIndex);
	}
}

GList* graph_get_edge_list_for_vertex(UndirectedGraph* g, int vertex) {
	return g_list_nth_data(g->edges, vertex);
}

void do_remove(GList* list, int data){
	GList* node = g_list_find_custom(list, &data, (GCompareFunc)gint_compare);
	if(node != NULL){
		//g_list_remove(list, node->data);
		g_list_remove_link(list, node);
		g_list_free_full(node, (GDestroyNotify)free);
	}
}

void graph_remove_edge(UndirectedGraph* g, int va, int vb){
	// get the edge list for va
	// find position for vb in the edge list for va
	GList* edgeList = graph_get_edge_list_for_vertex(g, va);
	do_remove(edgeList, vb);
	/*GList* data = g_list_find_custom(edgeList, &vb, (GCompareFunc)gint_compare);

	if(data != NULL){
		g_list_remove(edgeList, data->data);
	}*/

	/**
	 * Have to repeat for the opposite in case va=vb in which case calling this method twice
	 * would miss the second time.
	 */
	edgeList = graph_get_edge_list_for_vertex(g, vb);
	do_remove(edgeList, va);

}

void graph_print(UndirectedGraph* g) {

	printf(
			"numVertices: %ld, verticesA.length: %ld,  verticesB.length: %ld, edgeWeights.length: %ld, edges.length: %ld\n",
			g->numVertices, g->asize(), g->bsize(), g.esize(), g_list_length(g->edges));
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
	/*for (vector<vector<int> >::iterator itr = edges.begin(); itr != edges.end(); itr++) {
		vector<int> edge = *itr;
		printf("[");
		for(vector<int>::iterator it = edge.begin(); it != edge.end(); ++it){
			printf("%d, ", *it);
		}
		printf("]\n");
	}*/
}

