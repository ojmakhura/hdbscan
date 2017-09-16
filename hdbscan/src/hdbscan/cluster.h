/*
 * cluster.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include <glib.h>

#define CLUSTER_SUCCESS 1
#define CLUSTER_ERROR	0

typedef GList GSet;

typedef struct Cluster {
	int label;
	double birthLevel;
	double deathLevel;
	int numPoints;
	long offset; //First level where points with this cluster's label appear
	double stability;
	double propagatedStability;
	double propagatedLowestChildDeathLevel;
	int numConstraintsSatisfied;
	int propagatedNumConstraintsSatisfied;
	GSet* virtualChildCluster = NULL;
	cluster* parent;
	GList* propagatedDescendants = NULL;
	int hasChildren;
} cluster;

void cluster_destroy(cluster* cl);
//Cluster();
/**
 * Creates a new Cluster.
 * @param label The cluster label, which should be globally unique
 * @param parent The cluster which split to create this cluster
 * @param birthLevel The MST edge level at which this cluster first appeared
 * @param numPoints The initial number of points in this cluster
 */
int cluster_init(cluster* cl, int label, cluster* parent, double birthLevel, int numPoints);

/**
 * Removes the specified number of points from this cluster at the given edge level, which will
 * update the stability of this cluster and potentially cause cluster death.  If cluster death
 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
 * @param numPoints The number of points to remove from the cluster
 * @param level The MST edge level at which to remove these points
 */
void cluster_detach_points(cluster* cl, int numPoints, double level);

/**
 * This cluster will propagate itself to its parent if its number of satisfied constraints is
 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
 * propagated descendants.  In the case of ties, stability is examined.
 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
 * parent.
 */
void cluster_propagate(cluster* cl);

int cluster_add_points_to_virtual_child_cluster(cluster* cl, GSet* points);

GList* cluster_virtual_child_contains_point(cluster* cl, int point);

void cluster_add_virtual_child_constraints_satisfied(cluster* cl, int numConstraints);

void cluster_add_constraints_satisfied(cluster* cl, int numConstraints);

/**
 * Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
 * number of constraints satisfied by the virtual child cluster.
 */
void cluster_release_virtual_child(cluster* cl);

// ------------------------------ GETTERS & SETTERS ------------------------------

int cluster_get_label(cluster* cl){return cl->label;};
int getNumPoints();

cluster* getParent();

double getBirthLevel();

double getDeathLevel();

long getOffset();

void setOffset(long offset);

double getStability();
double getPropagatedStability();

double getPropagatedLowestChildDeathLevel();

int getNumConstraintsSatisfied();

int getPropagatedNumConstraintsSatisfied();

cluster* getPropagatedDescendants();

GSet* getVirtualChildCluster();

int hasKids();

//int operator==(Cluster* another);
/**
 *
 */
gint gint_compare(gconstpointer ptr_a, gconstpointer ptr_b);

#endif /* CLUSTER_H_ */
