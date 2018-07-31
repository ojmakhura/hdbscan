import PyHdbscan
import sys

def readFile(filename):
	f = open(filename)
	dataset = []
	for line in f:
		lineset = line.strip().split(",")
		
		row = []
		for l in line.strip().split(","):
			if len(l) > 0:
				row.append(float(l.strip()))
		
		if len(row) > 0:
			dataset.append(row)
	
	return dataset

def printLabels(labels):
	print("\nCluster labels = [");
	for l in labels:
		print(str(l) + " ", end="");
	print("]\n");
	
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
	
	scan = PyHdbscan.PyHdbscan(3)
	print("***********************************************************************************\n\n");
	print("Clustering for dataset with minPts =", 3)
	scan.run(dataset)
	printLabels(scan.labels)
	print(set(scan.labels))
	dmap = getClusterMap(scan.labels)
	
	for key in dmap:
		print(key, ":", dmap[key])
	
	print("\n***********************************************************************************\n");
	for i in range(4, 11):
		print("\nRe-Clustering for dataset with minPts =", i)
		scan.rerun(i)
		printLabels(scan.labels)
		print(set(scan.labels))
		dmap = getClusterMap(scan.labels) 
		
		for key in dmap:
			print(key, ":", dmap[key])
		print(getClusterMap(scan.labels))
		print("\n***********************************************************************************\n");

if __name__ == "__main__":
    main(sys.argv[1])
