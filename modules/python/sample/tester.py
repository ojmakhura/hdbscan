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

def main(fname):
	print("Opening file : ", fname)	
	dataset = readFile(fname)
	
	scan = PyHdbscan.PyHdbscan(3)
	print("***********************************************************************************\n\n");
	print("Clustering for dataset with minPts =", 3)
	scan.run(dataset)
	printLabels(scan.labels)
	print(set(scan.labels))
	print("\n***********************************************************************************\n");
	for i in range(4, 13):
		print("\nRe-Clustering for dataset with minPts =", i)
		scan.rerun(i)
		printLabels(scan.labels)
		print(set(scan.labels))
		print("\n***********************************************************************************\n");

if __name__ == "__main__":
    main(sys.argv[1])
