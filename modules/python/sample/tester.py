import PyHdbscan
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

def printLabels(labels):
	print("\nCluster labels = [")
	for l in labels:
		print(str(l) + " ", end="")
	print("]\n")
	
def getClusterMap(labels):
	dmap = {}
	for i in range(len(labels)):
		key = labels[i]
		if key in dmap:
			dmap[key].append(i)
		else:
			dmap[key] = [i]
	return dmap
	
def main(fname):
	print("Opening file : ", fname)	
	dataset = readFile(fname)
	
	scan = PyHdbscan.PyHdbscan(6)
		
	print("\n***********************************************************************************\n")
	for i in range(3, 4):
		if i == 3:
			print("Clustering for dataset with minPts =", 3)
			scan.run(dataset)
		else:
			print("\nRe-Clustering for dataset with minPts =", i)
			scan.rerun(i)
			
		printLabels(scan.labels)
		print(set(scan.labels))
		dmap = getClusterMap(scan.labels) 
		
		for key in dmap:
			print(key, ":", dmap[key])
		print(getClusterMap(scan.labels))
		print("\n***********************************************************************************\n")

if __name__ == "__main__":
    main(sys.argv[1])
