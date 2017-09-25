/*
 * cluster.cpp
 *
 *  Created on: 18 May 2016
 *      Author: junior
 */

#include "hdbscan/cluster.hpp"
#include <stdio.h>

using namespace std;
using namespace clustering;

namespace clustering {

Cluster::Cluster() {
	this->label = -1;
	this->birthLevel = -1;
	this->deathLevel = -1;
	this->numPoints = 0;
	this->offset = 0;
	this->stability = 0;
	this->propagatedStability = 0;
	this->propagatedLowestChildDeathLevel = numeric_limits<float>::max();
	this->numConstraintsSatisfied = 0;
	this->propagatedNumConstraintsSatisfied = 0;
	this->parent = NULL;
	this->hasChildren = false;
}

/**
 * The destructor
 *
 * Do not delete parent as it will be deleted when the
 * parent is being deleted
 */
Cluster::~Cluster() {

}

/**
 * Creates a new Cluster.
 * @param label The cluster label, which should be globally unique
 * @param parent The cluster which split to create this cluster
 * @param birthLevel The MST edge level at which this cluster first appeared
 * @param numPoints The initial number of points in this cluster
 */
Cluster::Cluster(int label, Cluster* parent, float birthLevel, int numPoints) {
	this->label = label;
	this->birthLevel = birthLevel;
	this->deathLevel = 0;
	this->numPoints = numPoints;
	this->offset = 0;

	this->stability = 0;
	this->propagatedStability = 0;

	this->propagatedLowestChildDeathLevel = numeric_limits<float>::max();

	this->numConstraintsSatisfied = 0;
	this->propagatedNumConstraintsSatisfied = 0;

	this->parent = parent;
	if (this->parent != NULL)
		this->parent->hasChildren = true;
	this->hasChildren = false;
}

/**
 * Removes the specified number of points from this cluster at the given edge level, which will
 * update the stability of this cluster and potentially cause cluster death.  If cluster death
 * occurs, the number of constraints satisfied by the virtual child cluster will also be calculated.
 * @param numPoints The number of points to remove from the cluster
 * @param level The MST edge level at which to remove these points
 */
void Cluster::detachPoints(int numPoints, float level) {
	//printf("Cluster::detachPoints: incoming points %d this points %d birthlevel %f\n", numPoints, this->numPoints, birthLevel);
	this->numPoints -= numPoints;
	this->stability += (numPoints * (1 / level - 1 / this->birthLevel));

	if (this->numPoints == 0)
		this->deathLevel = level;
	else if (this->numPoints < 0)
		throw Exception::IllegalStateException(
				"Cluster cannot have less than 0 points.");
}


/**
 * This cluster will propagate itself to its parent if its number of satisfied constraints is
 * higher than the number of propagated constraints.  Otherwise, this cluster propagates its
 * propagated descendants.  In the case of ties, stability is examined.
 * Additionally, this cluster propagates the lowest death level of any of its descendants to its
 * parent->
 */
void Cluster::propagate() {
	if (this->parent != NULL) {

		//Propagate lowest death level of any descendants:
		if (this->propagatedLowestChildDeathLevel == numeric_limits<float>::max()){
			this->propagatedLowestChildDeathLevel = this->deathLevel;
		}
		if (this->propagatedLowestChildDeathLevel < this->parent->propagatedLowestChildDeathLevel){
			this->parent->propagatedLowestChildDeathLevel = this->propagatedLowestChildDeathLevel;
		}

		//If this cluster has no children, it must propagate itself:
		if (!this->hasChildren) {
			this->parent->propagatedNumConstraintsSatisfied+= this->numConstraintsSatisfied;
			this->parent->propagatedStability+= this->stability;
			this->parent->propagatedDescendants.push_back(this);
		}

		else if (this->numConstraintsSatisfied > this->propagatedNumConstraintsSatisfied) {
			this->parent->propagatedNumConstraintsSatisfied+= this->numConstraintsSatisfied;
			this->parent->propagatedStability+= this->stability;
			this->parent->propagatedDescendants.push_back(this);
		}

		else if (this->numConstraintsSatisfied < this->propagatedNumConstraintsSatisfied) {
			//printf("other one:\n ");
			this->parent->propagatedNumConstraintsSatisfied+= this->propagatedNumConstraintsSatisfied;
			this->parent->propagatedStability+= this->propagatedStability;

			vector<Cluster*>* parentDescendants = this->parent->getPropagatedDescendants();
			parentDescendants->insert(parentDescendants->end(), this->propagatedDescendants.begin(), this->propagatedDescendants.end());
		}

		else if (this->numConstraintsSatisfied == this->propagatedNumConstraintsSatisfied) {
			//printf("Chose the parent over descendants if there is a tie in stability:\n ");
			//Chose the parent over descendants if there is a tie in stability:
			if (this->stability >= this->propagatedStability) {
				this->parent->propagatedNumConstraintsSatisfied+= this->numConstraintsSatisfied;
				this->parent->propagatedStability+= this->stability;
				this->parent->propagatedDescendants.push_back(this);
			}

			else {
				this->parent->propagatedNumConstraintsSatisfied+= this->propagatedNumConstraintsSatisfied;
				this->parent->propagatedStability+= this->propagatedStability;
				vector<Cluster*>* parentDescendants =
						this->parent->getPropagatedDescendants();

				parentDescendants->insert(parentDescendants->end(),	this->propagatedDescendants.begin(), this->propagatedDescendants.end());

			}
		}
	}
}


void Cluster::addPointsToVirtualChildCluster(set<int>& points) {

	this->virtualChildCluster.insert(points.begin(), points.end());

}


bool Cluster::virtualChildClusterContaintsPoint(int point) {

	return this->virtualChildCluster.find(point) != this->virtualChildCluster.end();
}


void Cluster::addVirtualChildConstraintsSatisfied(int numConstraints) {
	this->propagatedNumConstraintsSatisfied+= numConstraints;
}


void Cluster::addConstraintsSatisfied(int numConstraints) {
	this->numConstraintsSatisfied+= numConstraints;
}


/**
 * Sets the virtual child cluster to null, thereby saving memory.  Only call this method after computing the
 * number of constraints satisfied by the virtual child cluster.
 */
void Cluster::releaseVirtualChildCluster() {

	//printf("virtualChildCluster size %d\n", virtualChildCluster->size());
	if(parent != NULL){
		delete parent;
	}
}


// ------------------------------ PRIVATE METHODS ------------------------------

// ------------------------------ GETTERS & SETTERS ------------------------------

int Cluster::getLabel() {
	return this->label;
}

int Cluster::getNumPoints() {
	return this->numPoints;
}

Cluster* Cluster::getParent() {
	return this->parent;
}

float Cluster::getBirthLevel() {
	return this->birthLevel;
}

float Cluster::getDeathLevel() {
	return this->deathLevel;
}

long Cluster::getOffset() {
	return this->offset;
}

void Cluster::setOffset(long offset) {
	this->offset = offset;
}

float Cluster::getStability() {
	return this->stability;
}


float Cluster::getPropagatedStability(){
	return this->propagatedStability;
}
set<int>& Cluster::getVirtualChildCluster(){
	return this->virtualChildCluster;
}

float Cluster::getPropagatedLowestChildDeathLevel() {
	return this->propagatedLowestChildDeathLevel;
}

int Cluster::getNumConstraintsSatisfied() {
	return this->numConstraintsSatisfied;
}

int Cluster::getPropagatedNumConstraintsSatisfied() {
	return this->propagatedNumConstraintsSatisfied;
}

vector<Cluster*>* Cluster::getPropagatedDescendants() {
	return &this->propagatedDescendants;
}

bool Cluster::hasKids() {
	return this->hasChildren;
}

bool Cluster::operator==(Cluster* another){
	if(this->label == another->label){
		return true;
	}

	return false;
}

void Cluster::clean(){
	this->propagatedDescendants.clear();
	this->virtualChildCluster.clear();
}

namespace Exception{
IllegalStateException::IllegalStateException(char* error){
	this->error = error;
}

const char * IllegalStateException::what () const throw (){
	return error;
}
}

} /* namespace clustering */
