#include "hdbscan_Hdbscan.h"
#include "hdbscan/hdbscan.hpp"
#include <iostream>

using namespace clustering;
hdbscan scan;

JNIEXPORT void JNICALL Java_hdbscan_Hdbscan_initHdbscan(JNIEnv *env, jobject obj, jint minPoints){
	hdbscan_init(&scan, minPoints, DATATYPE_DOUBLE);
}

jintArray getLabelsArray(JNIEnv *env, int32_t *lbs, int rows){
	jintArray labels = env->NewIntArray(rows);	
	env->SetIntArrayRegion(labels, 0, rows, lbs);
	return labels;
}

JNIEXPORT jintArray JNICALL Java_hdbscan_Hdbscan_runImpl(JNIEnv *env, jobject obj, jobjectArray dataset){
	
	int rows = env->GetArrayLength(dataset);
	jdoubleArray dim=  (jdoubleArray)env->GetObjectArrayElement(dataset, 0);
	int cols = env -> GetArrayLength(dim);
	double *dset = new double[rows * cols];
	long dIdx = 0;

	for(int i = 0; i < rows; i++){
		dim =  (jdoubleArray)env->GetObjectArrayElement(dataset, i);
		jdouble *row = env->GetDoubleArrayElements(dim, NULL);
		
		for(int j = 0; j < cols; j++){
			dset[dIdx] = row[j];
			dIdx++;
		}
	}
	scan.run(dset, rows, cols, TRUE);
	
	delete dset;
	
	return getLabelsArray(env, scan.clusterLabels, rows);
}


JNIEXPORT jintArray JNICALL Java_hdbscan_Hdbscan_reRunImpl(JNIEnv *env, jobject obj, jint newMinPts){
	scan.reRun(newMinPts);	
	return getLabelsArray(env, scan.clusterLabels, scan.numPoints);
}

JNIEXPORT void JNICALL Java_hdbscan_Hdbscan_cleanHdbscan(JNIEnv *, jobject){
	//scan.clean();
}
