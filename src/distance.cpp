/*
 * distance.cpp
 *
 *  Created on: 25 Sep 2017
 *      Author: junior
 */


#ifdef __cplusplus
#include "hdbscan/distance.h"
#include <stdlib.h>
namespace clustering {

Distance::Distance(){
	distance_init(this, _EUCLIDEAN, DATATYPE_DOUBLE);
}

Distance::~Distance(){
	distance_destroy(this);
}

};
#endif

