/*
 * cluster.h
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
/** @file cluster.h */
#ifndef CLUSTER_H_
#define CLUSTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hdbscan/utils.h"
#include "gnulib/gl_array_oset.h"

#define CLUSTER_SUCCESS 1
#define CLUSTER_ERROR	0

#ifdef __cplusplus
namespace clustering {
#endif

/**
 * \struct Cluster
 * 
 * @brief Cluster object implementation
 * 
 * The cluster object has the label and the parent if it is child cluster as well
 * as descendants if it is not the las one.
 * 
 */
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
	gl_oset_t virtualChildCluster;
	struct Cluster* parent;
	ClusterList* propagatedDescendants;
	boolean hasChildren;

#ifdef __cplusplus
public:

	/**
	 * @brief Construct a new Cluster object
	 * 
	 */
	Cluster();
	
	/**
	 * @brief Construct a new Cluster object
	 * 
	 * @param label 
	 * @param parent 
	 * @param birthLevel 
	 * @param numPoints 
	 */
	Cluster(int32_t label, Cluster* parent, double birthLevel, int32_t numPoints);

	virtual ~Cluster();

	/**
	 * @brief Removes the specified number of points from this cluster at the given edge level, which will
	 * update the stability of this cluster and potentially cause cluster death.  If cluster death
	 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
	 * 
	 * @param numPoints 
	 * @param level 
	 */
	void detachPoints(int32_t numPoints, double level);

	/**
	 * @brief This cluster will propagate itself to its parent if its number of satisfied constraints is
	 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
	 * propagated descendants.  In the case of ties, stability is examined.
	 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
	 * parent.
	 */
	void propagate();

	/**
	 * @brief 
	 * 
	 * @param points 
	 */
	void addPointsToVirtualChildCluster(gl_oset_t points);

	/**
	 * @brief 
	 * 
	 * @param point 
	 * @return boolean 
	 */
	boolean virtualChildClusterContaintsPoint(int32_t point);

	/**
	 * @brief 
	 * 
	 * @param numConstraints 
	 */
	void addVirtualChildConstraintsSatisfied(int32_t numConstraints);

	/**
	 * @brief 
	 * 
	 * @param numConstraints 
	 */
	void addConstraintsSatisfied(int32_t numConstraints);


	/**
	 * @brief Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
	 * number of constraints satisfied by the virtual child cluster.
	 */
	void releaseVirtualChildCluster();

	// ------------------------------ GETTERS & SETTERS ------------------------------
	/**
	 * @brief Get the Label object
	 * 
	 * @return int32_t 
	 */
	int32_t getLabel() ;

	/**
	 * @brief Get the Num Points object
	 * 
	 * @return int32_t 
	 */
	int32_t getNumPoints() ;

	/**
	 * @brief Get the Parent cluster object
	 * 
	 * @return Cluster* 
	 */
	Cluster* getParent();

	/**
	 * @brief Get the Birth Level object
	 * 
	 * @return double 
	 */
	double getBirthLevel();

	/**
	 * @brief Get the Death Level object
	 * 
	 * @return double 
	 */
	double getDeathLevel();

	/**
	 * @brief Get the Offset object
	 * 
	 * @return long 
	 */
	long getOffset();

	/**
	 * @brief Set the Offset object
	 * 
	 * @param offset 
	 */
	void setOffset(long offset);

	/**
	 * @brief Get the Stability object
	 * 
	 * @return double 
	 */
	double getStability();

	/**
	 * @brief Get the Propagated Stability object
	 * 
	 * @return double 
	 */
	double getPropagatedStability();

	/**
	 * @brief Get the Propagated Lowest Child Death Level object
	 * 
	 * @return double 
	 */
	double getPropagatedLowestChildDeathLevel();

	/**
	 * @brief Get the Num Constraints Satisfied object
	 * 
	 * @return int32_t 
	 */
	int32_t getNumConstraintsSatisfied();

	/**
	 * @brief Get the Propagated Num Constraints Satisfied object
	 * 
	 * @return int32_t 
	 */
	int32_t getPropagatedNumConstraintsSatisfied();

	/**
	 * @brief Get the Propagated Descendants object
	 * 
	 * @return ClusterList* 
	 */
	ClusterList* getPropagatedDescendants();

	/**
	 * @brief Get the Virtual Child Cluster object
	 * 
	 * @return gl_oset_t 
	 */
	gl_oset_t getVirtualChildCluster();

	/**
	 * @brief 
	 * 
	 * @return boolean 
	 */
	boolean hasKids();

	/**
	 * @brief 
	 * 
	 * @param another 
	 * @return boolean 
	 */
	boolean operator==(Cluster* another);

	/**
	 * @brief Free all allocated memory
	 * 
	 */
	void clean();

#endif

};

/**\typedef cluster */
typedef struct Cluster cluster;

/**
 * @brief Free all the memory allocated for the cluster and set it to NULL
 * 
 * @param cl The cluster to destroy
 */
void cluster_destroy(cluster* cl);

/**
 * @brief Creates a new Cluster and allocate all necessary memory.
 * 
 * @param cl The cluster to initialise. It can be NULL
 * @param label The cluster label, which should be globally unique
 * @param parent The cluster which split to create this cluster
 * @param birthLevel The MST edge level at which this cluster first appeared
 * @param numPoints The initial number of points in this cluster
 * @return cluster* Initialised cluster
 */
cluster* cluster_init(cluster* cl, int32_t label, cluster* parent, double birthLevel, int32_t numPoints);

/**
 * @brief Removes the specified number of points from this cluster at the given edge level
 * 
 * This will update the stability of this cluster and potentially cause cluster death.  If cluster death
 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
 * 
 * @param cl 
 * @param numPoints The number of points to remove from the cluster
 * @param level The MST edge level at which to remove these points
 * @return int 
 */
int cluster_detach_points(cluster* cl, int32_t numPoints, double level);

/**
 * @brief Propagation of the cluster ot its parent
 * 
 * This cluster will propagate itself to its parent if its number of satisfied constraints is
 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
 * propagated descendants. In the case of ties, stability is examined.
 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
 * parent.
 * 
 * @param cl 
 */
void cluster_propagate(cluster* cl);

/**
 * @brief 
 * 
 * @param cl 
 * @param points 
 * @return int32_t 
 */
int32_t cluster_add_points_to_virtual_child_cluster(cluster* cl, gl_oset_t points);

/**
 * @brief 
 * 
 * @param cl 
 * @param point 
 * @return boolean 
 */
boolean cluster_virtual_child_contains_point(cluster* cl, int32_t point);

/**
 * @brief 
 * 
 * @param cl 
 * @param numConstraints 
 */
void cluster_add_virtual_child_constraints_satisfied(cluster* cl, int32_t numConstraints);

/**
 * @brief 
 * 
 * @param cl 
 * @param numConstraints 
 */
void cluster_add_constraints_satisfied(cluster* cl, int32_t numConstraints);

/**
 * @brief Sets the virtual child cluster to null, thereby saving memory.  
 * 
 * Only call this method after computing the number of constraints satisfied 
 * by the virtual child cluster.
 * 
 * @param cl 
 */
void cluster_release_virtual_child(cluster* cl);

// ------------------------------ GETTERS & SETTERS ------------------------------


#ifdef __cplusplus
};
}
#endif
#endif /* CLUSTER_H_ */
