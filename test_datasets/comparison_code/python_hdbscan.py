# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import hdbscan
import numpy as np
import math
import sys

def readFile(filename):
	dataset = np.genfromtxt(filename, delimiter=',')
	r,c = dataset.shape
	
	if math.isnan(dataset[0][c-1]):
		dataset = np.delete(dataset, -1, axis=1)
	print("rows = ", r, " coloumns = ", c)
	return dataset
		
def clusterMap(labels):
	cmap = dict()
	for i in range(labels.shape[0]):
		label = labels[i]
		if label not in cmap:
			cmap[label] = list()
		
		cmap[label].append(i)
	
	return cmap
	
def main(fname, minPts):
	print("Opening file : ", fname)
	
	sps = fname.split("/")
	sps[len(sps)-1] = "clusters/python_" + str(minPts) + "_" + sps[len(sps)-1]
	newName = "/".join(sps)
	print("New name is " + newName)
	
	dataset = readFile(fname)
	
	scan = hdbscan.HDBSCAN(min_cluster_size=minPts)
		
	print("\n***********************************************************************************\n")
	labels = scan.fit_predict(dataset)
	cmap = clusterMap(labels)
	
	fp = open(newName, "w")
		
	for k, v in cmap.items():
		tmp = np.array([k] + v)
		ts = ",".join(tmp.astype(str))
		print(ts)
		fp.write(ts + "\n")
		
	fp.close()
	#print("\n***********************************************************************************\n")
	#print(scan.outlier_scores_)
	#print("\n***********************************************************************************\n")

if __name__ == "__main__":
    main(sys.argv[1], int(sys.argv[2]))

