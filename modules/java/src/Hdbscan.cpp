/*
 * Hdbscan.cpp
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
 * @file Hdbscan.cpp
 * 
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * 
 * @brief Implementation of the JNI functions.
 * 
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "hdbscan_Hdbscan.h"
#include "hdbscan/hdbscan.hpp"
#include <iostream>

using namespace clustering;
hdbscan scan;

JNIEXPORT void JNICALL Java_hdbscan_Hdbscan_initHdbscan(JNIEnv *env, jobject obj, jint minPoints){
	hdbscan_init(&scan, minPoints);
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
	scan.run(dset, rows, cols, TRUE, H_DOUBLE);
	
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
