/*
 * cluster.h
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

//#include <boost/exception/all.hpp>
#include <vector>
#include <cstddef>
#include <limits>

#include <set>

using namespace std;

namespace clustering {

class Cluster {

private:
	int label;
	double birthLevel;
	double deathLevel;
	int numPoints;
	long offset;//First level where points with this cluster's label appear
	double stability;
	double propagatedStability;
	double propagatedLowestChildDeathLevel;
	int numConstraintsSatisfied;
	int propagatedNumConstraintsSatisfied;
	set<int>* virtualChildCluster;
	Cluster* parent;
	vector<Cluster*>* propagatedDescendants;

public:
	bool hasChildren;
	Cluster();
	/**
	 * Creates a new Cluster.
	 * @param label The cluster label, which should be globally unique
	 * @param parent The cluster which split to create this cluster
	 * @param birthLevel The MST edge level at which this cluster first appeared
	 * @param numPoints The initial number of points in this cluster
	 */
	Cluster(int label, Cluster* parent, double birthLevel, int numPoints);

	virtual ~Cluster();

	/**
	 * Removes the specified number of points from this cluster at the given edge level, which will
	 * update the stability of this cluster and potentially cause cluster death.  If cluster death
	 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
	 * @param numPoints The number of points to remove from the cluster
	 * @param level The MST edge level at which to remove these points
	 */
	void detachPoints(int numPoints, double level);


	/**
	 * This cluster will propagate itself to its parent if its number of satisfied constraints is
	 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
	 * propagated descendants.  In the case of ties, stability is examined.
	 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
	 * parent.
	 */
	void propagate();


	void addPointsToVirtualChildCluster(set<int>* points);


	bool virtualChildClusterContaintsPoint(int point);


	void addVirtualChildConstraintsSatisfied(int numConstraints);

	void addConstraintsSatisfied(int numConstraints);


	/**
	 * Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
	 * number of constraints satisfied by the virtual child cluster.
	 */
	void releaseVirtualChildCluster();

	// ------------------------------ GETTERS & SETTERS ------------------------------

	int getLabel() ;
	int getNumPoints() ;

	Cluster* getParent();

	double getBirthLevel();

	double getDeathLevel();

	long getOffset();

	void setOffset(long offset);

	double getStability();
	double getPropagatedStability();

	double getPropagatedLowestChildDeathLevel();

	int getNumConstraintsSatisfied();

	int getPropagatedNumConstraintsSatisfied();

	vector<Cluster*>* getPropagatedDescendants();

	set<int>* getVirtualChildCluster();

	 bool hasKids();

	 bool operator==(Cluster* another);
};

namespace Exception{

class IllegalStateException : public std::exception{

private:
	char* error;

public:
	IllegalStateException(char* error);

	const char * what () const throw ();

};

}

} /* namespace clustering */

#endif /* CLUSTER_H_ */
