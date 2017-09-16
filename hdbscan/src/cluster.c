/*
 * cluster.c
 *
 *  Created on: 13 Sep 2017
 *      Author: ojmakh
 */
#include "hdbscan/cluster.h"
#include <float.h>

int cluster_init(cluster* cl, int label, cluster* parent, double birthLevel, int numPoints){
	cl = (cluster*)malloc(sizeof(cluster));

	if(cl == NULL){
		return CLUSTER_ERROR;
	}

	cl->label = label;
	cl->birthLevel = birthLevel;
	cl->deathLevel = 0;
	cl->numPoints = numPoints;
	//this->offset = 0;

	cl->stability = 0;
	cl->propagatedStability = 0;

	cl->propagatedLowestChildDeathLevel = DBL_MAX;

	cl->numConstraintsSatisfied = 0;
	cl->propagatedNumConstraintsSatisfied = 0;
	cl->parent = parent;
	if (cl->parent != NULL)
		cl->parent->hasChildren = 1;
	cl->hasChildren = 0;
	cl->virtualChildCluster = NULL;
	cl->propagatedDescendants = NULL;

	return CLUSTER_SUCCESS;
}

void cluster_destroy(cluster* cl){
	g_list_free_full(cl->propagatedDescendants, (GDestroyNotify)free);
	g_list_free_full(cl->virtualChildCluster, (GDestroyNotify)free);

	free(cl);
}


void cluster_detach_points(cluster* cl, int numPoints, double level){

	//printf("Cluster::detachPoints: incoming points %d cl points %d birthlevel %f\n", numPoints, cl->numPoints, birthLevel);
	cl->numPoints -= numPoints;
	cl->stability += (numPoints * (1 / level - 1 / cl->birthLevel));

	if (cl->numPoints == 0)
		cl->deathLevel = level;
	else if (cl->numPoints < 0){
		printf("Cluster cannot have less than 0 points.");
		exit(-1);
	}
}

void cluster_propagate(cluster* cl){

	if (cl->parent != NULL) {
		//GList* parentDescendants = cl->parent->propagatedDescendants;

		//Propagate lowest death level of any descendants:
        if (cl->propagatedLowestChildDeathLevel == DBL_MAX){
        	cl->propagatedLowestChildDeathLevel = cl->deathLevel;
		}

		if (cl->propagatedLowestChildDeathLevel < cl->parent->propagatedLowestChildDeathLevel){
			cl->parent->propagatedLowestChildDeathLevel = cl->propagatedLowestChildDeathLevel;
		}

		//If cl cluster has no children, it must propagate itself:
		if (!cl->hasChildren) {
			cl->parent->propagatedNumConstraintsSatisfied+= cl->numConstraintsSatisfied;
			cl->parent->propagatedStability+= cl->stability;
			cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
			//dlist_ins_next(parentDescendants, dlist_tail(parentDescendants), cl);
		}

		else if (cl->numConstraintsSatisfied > cl->propagatedNumConstraintsSatisfied) {
			cl->parent->propagatedNumConstraintsSatisfied+= cl->numConstraintsSatisfied;
			cl->parent->propagatedStability+= cl->stability;
			cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
		}

		else if (cl->numConstraintsSatisfied < cl->propagatedNumConstraintsSatisfied) {
			//printf("other one:\n ");
			cl->parent->propagatedNumConstraintsSatisfied+= cl->propagatedNumConstraintsSatisfied;
			cl->parent->propagatedStability+= cl->propagatedStability;
			cl->parent->propagatedDescendants = g_list_concat(cl->parent->propagatedDescendants, cl->propagatedDescendants);
		}

		else if (cl->numConstraintsSatisfied == cl->propagatedNumConstraintsSatisfied) {
			//printf("Chose the parent over descendants if there is a tie in stability:\n ");
			//Chose the parent over descendants if there is a tie in stability:
			if (cl->stability >= cl->propagatedStability) {
				cl->parent->propagatedNumConstraintsSatisfied+= cl->numConstraintsSatisfied;
				cl->parent->propagatedStability+= cl->stability;
				cl->parent->propagatedDescendants = g_list_append(cl->parent->propagatedDescendants, cl);
			}

			else {
				cl->parent->propagatedNumConstraintsSatisfied+= cl->propagatedNumConstraintsSatisfied;
				cl->parent->propagatedStability+= cl->propagatedStability;
				cl->parent->propagatedDescendants = g_list_concat(cl->parent->propagatedDescendants, cl->propagatedDescendants);
			}
		}
	}
}

gint gint_compare(gconstpointer ptr_a, gconstpointer ptr_b) {
	gint a, b;
	a = *(gint *) ptr_a;
	b = *(gint *) ptr_b;

	if (a > b) {
		return (1);
	}
	if (a == b) {
		return (0);
	}
	/* default: a < b */
	return (-1);
}

int cluster_add_points_to_virtual_child_cluster(cluster* cl, GSet* points){

	int size = g_list_length(points);

	for(int i = 0; i < size; i++){
		int* d = (int *)g_list_nth_data(points, i);
		GList *search = g_list_find_custom(cl->virtualChildCluster, d, (GCompareFunc)gint_compare);
		if(search == NULL){
			int *data;
			if ((data = (int *)malloc(sizeof(int))) == NULL){
			      return 0;
			}
			*data = *d;
			cl->virtualChildCluster = g_list_append(cl->virtualChildCluster, data);
			cl->virtualChildCluster = g_list_sort(cl->virtualChildCluster, (GCompareFunc)gint_compare);
		}
	}

	return 1;
}

GList* cluster_virtual_child_contains_point(cluster* cl, int point){

	return g_list_find_custom(cl->virtualChildCluster, &point, (GCompareFunc)gint_compare);
}

void cluster_add_virtual_child_constraints_satisfied(cluster* cl, int numConstraints){

	cl->propagatedNumConstraintsSatisfied += numConstraints;
}

void cluster_add_constraints_satisfied(cluster* cl, int numConstraints){

	cl->numConstraintsSatisfied += numConstraints;
}

void cluster_release_virtual_child(cluster* cl){

	if(cl->parent != NULL){
		cluster_clean(cl->parent);
	}
}

