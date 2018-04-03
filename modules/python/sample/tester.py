import PyHdbscan
import faulthandler
faulthandler.enable()
data = [[1,1,3,3], [1,2,8,18], [2,1,8,18], [2,2,3,5], [2,4,5,3], [3,1,3,4], [3,2,9,18], [3,3,9,19], [3,4,9,20], [3,5,4,3], [3,6,20,2], [3,7,10,44], [4,2,4,5], [4,3,3,5], [4,5,10,18], [4,6,10,18], [5,1,3,4], [5,4,5,5], [6,0,4,4], [6,1,4,4], [6,2,8,19], [6,3,8,18], [6,4,1,20]]
scan = PyHdbscan.PyHdbscan(3)
print("Clustering for minPts =", 3)
scan.run(data)
print(scan.labels)

for i in range(4, 14):
	print("Re-clustering for minPts =", i)
	scan.rerun(i)
	print(scan.labels)

