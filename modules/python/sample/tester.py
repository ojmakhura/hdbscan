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
	
def main(fname, minPts):
	print("Opening file : ", fname)	
	dataset = readFile(fname)
	print(type(dataset))
	
	scan = PyHdbscan.PyHdbscan(minPts)
		
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
		scan.getClusterMap(0, scan.rows)
		
		for key in scan.clusterMap:
			print(key, ":", scan.clusterMap[key], "\n")
        
		print("\n***********************************************************************************\n")

if __name__ == "__main__":
    main(sys.argv[1], int(sys.argv[2]))
