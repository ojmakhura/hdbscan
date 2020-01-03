/*
 * cluster.c
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
 * @file cluster.c
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the functions from cluster.h
 * 
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "hdbscan/cluster.h"
#include "hdbscan/logger.h"

#include <float.h>

/**
 * @brief Create a new cluster
 * 
 * @param cl 
 * @param label The cluster label which should be globally unique
 * @param parent The cluster which split to create this cluster.
 * @param birthLevel The MST edge level at which this cluster first appears
 * @param numPoints The number of points in this cluster.
 * @return cluster* 
 */
cluster* cluster_init(cluster* cl, label_t label, cluster* parent, distance_t birthLevel, index_t numPoints){
	
	if(cl == NULL){
		cl = (cluster*)malloc(sizeof(cluster));
	}
	if(cl == NULL){
		logger_write(ERROR, "cluster_init - Could not allocate memory for cluster.");	
	} else {

		cl->label = label;
		cl->birthLevel = birthLevel;
		cl->deathLevel = 0;
		cl->numPoints = numPoints;
		cl->offset = 0;
		cl->stability = 0;
		cl->propagatedStability = 0;
		cl->propagatedLowestChildDeathLevel = D_MAX;
		cl->numConstraintsSatisfied = 0;
		cl->propagatedNumConstraintsSatisfied = 0;
		cl->parent = parent;
		if (cl->parent != NULL)
			cl->parent->hasChildren = TRUE;
		cl->hasChildren = FALSE;
		cl->virtualChildCluster = set_init(sizeof(index_t), NULL);
		if(sizeof(index_t) == sizeof(int)) {
			cl->virtualChildCluster->compare = int_compare;
		} else if(sizeof(index_t) == sizeof(long)) {
			cl->virtualChildCluster->compare = long_compare;
		} else {
			cl->virtualChildCluster->compare = short_compare;
		}

		cl->propagatedDescendants = ptr_array_list_init(1, cluster_compare);
	}

	return cl;
}

/**
 * @brief Free all the memory allocated for the cluster.
 * 
 * @param cl 
 */
void cluster_destroy(cluster* cl){
	if(cl != NULL){
		if(cl->virtualChildCluster != NULL){
			set_delete(cl->virtualChildCluster);
			cl->virtualChildCluster = NULL;
		}

		if(cl->propagatedDescendants != NULL){
			array_list_delete(cl->propagatedDescendants);
			cl->propagatedDescendants = NULL;
		}
		free(cl);
	}
} 

/**
 * @brief Removes the specified number of points from this cluster at the given edge level, which will
 * update the stability of this cluster and potentially cause cluster death.  If cluster death
 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
 * 
 * @param cl 
 * @param numPoints 
 * @param level 
 * @return int 
 */
int cluster_detach_points(cluster* cl, index_t numPoints, distance_t level){

	cl->numPoints = (index_t)(cl->numPoints - numPoints);
	cl->stability += (numPoints * (1 / level - 1 / cl->birthLevel));

	if (cl->numPoints == 0)
		cl->deathLevel = level;
	else if (cl->numPoints < 0){
		
		char s[100];
		sprintf(s, "cluster_detach_points - Cluster %d has %d points.\n", cl->label, cl->numPoints);
		logger_write(FATAL, s);
		return CLUSTER_ERROR;
	}

	return CLUSTER_SUCCESS;
}

/**
 * @brief This cluster will propagate itself to its parent if its number of satisfied constraints is
 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
 * propagated descendants.  In the case of ties, stability is examined.
 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
 * parent.
 * 
 * @param cl 
 */
void cluster_propagate(cluster* cl){

	if (cl->parent != NULL) {
		//Propagate lowest death level of any descendants:
        if (cl->propagatedLowestChildDeathLevel == D_MAX){
        	cl->propagatedLowestChildDeathLevel = cl->deathLevel;
		}

		if (cl->propagatedLowestChildDeathLevel < cl->parent->propagatedLowestChildDeathLevel){
			cl->parent->propagatedLowestChildDeathLevel = cl->propagatedLowestChildDeathLevel;
		}

		if (cl->hasChildren == FALSE || cl->numConstraintsSatisfied > cl->propagatedNumConstraintsSatisfied) {
			cl->parent->propagatedNumConstraintsSatisfied = (index_t) (cl->parent->propagatedNumConstraintsSatisfied + cl->numConstraintsSatisfied);
			cl->parent->propagatedStability += cl->stability;
			array_list_append(cl->parent->propagatedDescendants, &cl);
		}
		else if (cl->numConstraintsSatisfied < cl->propagatedNumConstraintsSatisfied) {

			cl->parent->propagatedNumConstraintsSatisfied = (index_t) (cl->parent->propagatedNumConstraintsSatisfied + cl->propagatedNumConstraintsSatisfied);
			cl->parent->propagatedStability += cl->propagatedStability;

			for(index_t i = 0; i < cl->propagatedDescendants->size; i++)
			{
				cluster* c = NULL;
				array_list_value_at(cl->propagatedDescendants, i, &c);
				array_list_append(cl->parent->propagatedDescendants, &c);
			}
		}
		else if (cl->numConstraintsSatisfied == cl->propagatedNumConstraintsSatisfied) {
			//Chose the parent over descendants if there is a tie in stability:
			if (cl->stability >= cl->propagatedStability) {
				cl->parent->propagatedNumConstraintsSatisfied = (index_t)(cl->parent->propagatedNumConstraintsSatisfied + cl->numConstraintsSatisfied);
				cl->parent->propagatedStability += cl->stability;
				array_list_append(cl->parent->propagatedDescendants, &cl);
			}

			else {
				cl->parent->propagatedNumConstraintsSatisfied = (index_t)(cl->parent->propagatedNumConstraintsSatisfied + cl->propagatedNumConstraintsSatisfied);
				cl->parent->propagatedStability += cl->propagatedStability;

				for(index_t i = 0; i < cl->propagatedDescendants->size; i++)
				{
					cluster *c = NULL;
					array_list_value_at(cl->propagatedDescendants, i, &c);
					array_list_append(cl->parent->propagatedDescendants, &c);
				}
			}
		}
	}
}


int cluster_add_points_to_virtual_child_cluster(cluster* cl, set_t* points){
	index_t d;
	for(index_t i = 0; i < points->size; i++){
		
		set_value_at(points, i, &d);
		set_insert(cl->virtualChildCluster, &d);
	}
	
	return 1;
}

boolean cluster_virtual_child_contains_point(cluster* cl, index_t point){
	if(set_find(cl->virtualChildCluster, &point) >= 0) {
		return 1;
	} 
	return 0;
}

void cluster_add_virtual_child_constraints_satisfied(cluster* cl, index_t numConstraints){

	cl->propagatedNumConstraintsSatisfied = (index_t)(cl->propagatedNumConstraintsSatisfied + numConstraints);
}

void cluster_add_constraints_satisfied(cluster* cl, index_t numConstraints){

	cl->numConstraintsSatisfied = (index_t)(cl->numConstraintsSatisfied + numConstraints);
}

void cluster_release_virtual_child(cluster* cl){

	if(cl->parent != NULL){
		cluster_destroy(cl->parent);
	}
}

int32_t cluster_compare(const void* a, const void* b)
{
	const cluster* c1 = (cluster *)a;
	const cluster* c2 = (cluster *)b;

	if(c1 == NULL)
	{
		return -1;
	}

	if(c2 == NULL)
	{
		return 1;
	}
	
	if (c1->label > c2->label) {
		return 1;
	}
	if (c1->label == c2->label) {
		return 0;
	}
	
	return -1;
}