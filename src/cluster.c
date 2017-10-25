/*
 * cluster.c
 *
 *  Created on: 13 Sep 2017
 *      Author: ojmakh
 */
#include "config.h"
#include "hdbscan/cluster.h"

#include <float.h>

cluster* cluster_init(cluster* cl, int32_t label, cluster* parent, double birthLevel, int32_t numPoints){
	if(cl == NULL){
		cl = (cluster*)malloc(sizeof(cluster));
	}
	if(cl == NULL){
		printf("ERROR: cluster_init - Could not allocate memory for cluster");
	}{

		cl->label = label;
		cl->birthLevel = birthLevel;
		cl->deathLevel = 0;
		cl->numPoints = numPoints;
		cl->offset = 0;

		cl->stability = 0;
		cl->propagatedStability = 0;

		cl->propagatedLowestChildDeathLevel = DBL_MAX;

		cl->numConstraintsSatisfied = 0;
		cl->propagatedNumConstraintsSatisfied = 0;
		cl->parent = parent;
		if (cl->parent != NULL)
			cl->parent->hasChildren = TRUE;
		cl->hasChildren = FALSE;
		cl->virtualChildCluster = gl_oset_nx_create_empty (GL_ARRAY_OSET, (gl_setelement_compar_fn) int_compare, NULL);
		cl->propagatedDescendants = NULL;
	}

	return cl;
}

void cluster_destroy(cluster* cl){
//#ifdef __cplusplus
	if(cl != NULL){
		if(cl->virtualChildCluster){
			gl_oset_free(cl->virtualChildCluster);
		}

		if(cl->propagatedDescendants){
			ListNode* node = g_list_first(cl->propagatedDescendants);

			while(node != NULL){
				node->data = NULL;
				node = g_list_next(node);
			}
			//printf("cl->propagatedDescendants has %d clusters\n", g_list_length(cl->propagatedDescendants));
			g_list_free(cl->propagatedDescendants);
		}
		free(cl);
	}
//#endif
}


int cluster_detach_points(cluster* cl, int numPoints, double level){

	//printf("Cluster::detachPoints: incoming points %d cl points %d birthlevel %f\n", numPoints, cl->numPoints, cl->birthLevel);
	cl->numPoints -= numPoints;
	cl->stability += (numPoints * (1 / level - 1 / cl->birthLevel));

	if (cl->numPoints == 0)
		cl->deathLevel = level;
	else if (cl->numPoints < 0){
		printf("ERROR: Cluster %d has %d points.\n", cl->label, cl->numPoints);
		return CLUSTER_ERROR;
	}

	return CLUSTER_SUCCESS;
}

void cluster_propagate(cluster* cl){

	if (cl->parent != NULL) {
		//Propagate lowest death level of any descendants:
        if (cl->propagatedLowestChildDeathLevel == DBL_MAX){
        	cl->propagatedLowestChildDeathLevel = cl->deathLevel;
		}

		if (cl->propagatedLowestChildDeathLevel < cl->parent->propagatedLowestChildDeathLevel){
			cl->parent->propagatedLowestChildDeathLevel = cl->propagatedLowestChildDeathLevel;
		}

		if (cl->hasChildren == FALSE) {
			cl->parent->propagatedNumConstraintsSatisfied += cl->numConstraintsSatisfied;
			cl->parent->propagatedStability += cl->stability;
			cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
		}
		else if (cl->numConstraintsSatisfied > cl->propagatedNumConstraintsSatisfied) {
			cl->parent->propagatedNumConstraintsSatisfied += cl->numConstraintsSatisfied;
			cl->parent->propagatedStability += cl->stability;
			cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
		}

		else if (cl->numConstraintsSatisfied < cl->propagatedNumConstraintsSatisfied) {
			//printf("other one:\n ");
			cl->parent->propagatedNumConstraintsSatisfied += cl->propagatedNumConstraintsSatisfied;
			cl->parent->propagatedStability += cl->propagatedStability;
			cl->parent->propagatedDescendants = g_list_concat(cl->parent->propagatedDescendants, cl->propagatedDescendants);
		}

		else if (cl->numConstraintsSatisfied == cl->propagatedNumConstraintsSatisfied) {
			//Chose the parent over descendants if there is a tie in stability:
			if (cl->stability >= cl->propagatedStability) {
				cl->parent->propagatedNumConstraintsSatisfied += cl->numConstraintsSatisfied;
				cl->parent->propagatedStability += cl->stability;
				cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
			}

			else {
				cl->parent->propagatedNumConstraintsSatisfied += cl->propagatedNumConstraintsSatisfied;
				cl->parent->propagatedStability += cl->propagatedStability;
				cl->parent->propagatedDescendants = g_list_concat(cl->parent->propagatedDescendants, cl->propagatedDescendants);
			}
		}
	}
}


int cluster_add_points_to_virtual_child_cluster(cluster* cl, gl_oset_t points){
	
	for(size_t i = 0; i < points->count; i++){
		int32_t d;
		gl_oset_value_at(points, i, &d);
		gl_oset_nx_add(cl->virtualChildCluster, d);
	}

	/**ListNode* node = g_list_first(points);
	guint s = 0; // dummy size we won't be using for anything

	while(node != NULL){

		int *d = (int *)node->data;
		cl->virtualChildCluster = set_int_insert(cl->virtualChildCluster, *d, &s);

		node = node->next;
	}*/

	return 1;
}

boolean cluster_virtual_child_contains_point(cluster* cl, int32_t point){
	//ListNode* node = g_list_find_custom(cl->virtualChildCluster, &point, (GCompareFunc)gint_compare);

	return gl_oset_search(cl->virtualChildCluster, point);
}

void cluster_add_virtual_child_constraints_satisfied(cluster* cl, int32_t numConstraints){

	cl->propagatedNumConstraintsSatisfied += numConstraints;
}

void cluster_add_constraints_satisfied(cluster* cl, int32_t numConstraints){

	cl->numConstraintsSatisfied += numConstraints;
}

void cluster_release_virtual_child(cluster* cl){

	if(cl->parent != NULL){
		cluster_destroy(cl->parent);
	}
}

