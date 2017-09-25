/*
 * cluster.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include "utils.h"

#define CLUSTER_SUCCESS 1
#define CLUSTER_ERROR	0

#ifdef __cplusplus
namespace clustering {
#endif

struct Cluster {

	int32_t label;
	double birthLevel;
	double deathLevel;
	int32_t numPoints;
	int64_t offset; //First level where points with this cluster's label appear
	double stability;
	double propagatedStability;
	double propagatedLowestChildDeathLevel;
	int32_t numConstraintsSatisfied;
	int32_t propagatedNumConstraintsSatisfied;
	IntSet* virtualChildCluster;
	struct Cluster* parent;
	ClusterList* propagatedDescendants;
	boolean hasChildren;

#ifdef __cplusplus
public:

	Cluster();
	/**
	 * Creates a new Cluster.
	 * @param label The cluster label, which should be globally unique
	 * @param parent The cluster which split to create this cluster
	 * @param birthLevel The MST edge level at which this cluster first appeared
	 * @param numPoints The initial number of points in this cluster
	 */
	Cluster(int32_t label, Cluster* parent, double birthLevel, int32_t numPoints);

	virtual ~Cluster();

	/**
	 * Removes the specified number of points from this cluster at the given edge level, which will
	 * update the stability of this cluster and potentially cause cluster death.  If cluster death
	 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
	 * @param numPoints The number of points to remove from the cluster
	 * @param level The MST edge level at which to remove these points
	 */
	void detachPoints(int32_t numPoints, double level);


	/**
	 * This cluster will propagate itself to its parent if its number of satisfied constraints is
	 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
	 * propagated descendants.  In the case of ties, stability is examined.
	 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
	 * parent.
	 */
	void propagate();


	void addPointsToVirtualChildCluster(IntSet* points);


	boolean virtualChildClusterContaintsPoint(int32_t point);


	void addVirtualChildConstraintsSatisfied(int32_t numConstraints);

	void addConstraintsSatisfied(int32_t numConstraints);


	/**
	 * Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
	 * number of constraints satisfied by the virtual child cluster.
	 */
	void releaseVirtualChildCluster();

	// ------------------------------ GETTERS & SETTERS ------------------------------

	int32_t getLabel() ;
	int32_t getNumPoints() ;

	Cluster* getParent();

	double getBirthLevel();

	double getDeathLevel();

	long getOffset();

	void setOffset(long offset);

	double getStability();
	double getPropagatedStability();

	double getPropagatedLowestChildDeathLevel();

	int32_t getNumConstraintsSatisfied();

	int32_t getPropagatedNumConstraintsSatisfied();

	ClusterList* getPropagatedDescendants();

	IntSet* getVirtualChildCluster();

	 boolean hasKids();

	 boolean operator==(Cluster* another);

	 void clean();

#endif

};

typedef struct Cluster cluster;


void cluster_destroy(cluster* cl);
//Cluster();
/**
 * Creates a new Cluster.
 * @param label The cluster label, which should be globally unique
 * @param parent The cluster which split to create this cluster
 * @param birthLevel The MST edge level at which this cluster first appeared
 * @param numPoints The initial number of points in this cluster
 */
cluster* cluster_init(cluster* cl, int32_t label, cluster* parent, double birthLevel, int32_t numPoints);

/**
 * Removes the specified number of points from this cluster at the given edge level, which will
 * update the stability of this cluster and potentially cause cluster death.  If cluster death
 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
 * @param numPoints The number of points to remove from the cluster
 * @param level The MST edge level at which to remove these points
 */
int cluster_detach_points(cluster* cl, int32_t numPoints, double level);

/**
 * This cluster will propagate itself to its parent if its number of satisfied constraints is
 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
 * propagated descendants.  In the case of ties, stability is examined.
 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
 * parent.
 */
void cluster_propagate(cluster* cl);

int32_t cluster_add_points_to_virtual_child_cluster(cluster* cl, IntSet* points);

boolean cluster_virtual_child_contains_point(cluster* cl, int32_t point);

void cluster_add_virtual_child_constraints_satisfied(cluster* cl, int32_t numConstraints);

void cluster_add_constraints_satisfied(cluster* cl, int32_t numConstraints);

/**
 * Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
 * number of constraints satisfied by the virtual child cluster.
 */
void cluster_release_virtual_child(cluster* cl);

// ------------------------------ GETTERS & SETTERS ------------------------------


#ifdef __cplusplus
};
}
#endif
#endif /* CLUSTER_H_ */
