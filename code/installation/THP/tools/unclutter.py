# import cv2
import numpy as np
from glob import glob

files = glob("../bin/data/embeddings/vgg-features/0.001_02.npy")

def dist(p,q):
	return ((p[0]-q[0])**2+(p[1]-q[1])**2)**0.5

def iterate(pts):
	fs = []
	vs = []
	D = 0.05
	for i in range(len(pts)):
		fs.append(np.array([0.0,0.0]))
		vs.append(np.array([0.0,0.0]))

	for i in range(len(pts)):
		if i % 10 == 0:
			print(i)
		for j in range(i+1,len(pts)):
			if abs(pts[i][0]-pts[j][0]) > D:
				continue
			if abs(pts[i][1]-pts[j][1]) > D:
				continue
			d = dist(pts[i],pts[j])
			
			if d < D:
				# print(d)
				x = (pts[i]-pts[j])/d
				# print(x,d)
				y = - x * (D-d)
				fs[i] += y
				fs[j] -= y

	for i in range(len(pts)):
		vs[i] += fs[i]*0.5

	for i in range(len(pts)):
		pts[i] += vs[i]

for f in files:
	print(f)
	npy = np.load(f)
	
	print(npy.shape[0])
	pts = []
	for i in range(npy.shape[0]):
		pts.append(np.array([npy[i][0],npy[i][1]]))

	for i in range(10):
		print(i)
		iterate(pts)


		s = ""
		for j in range(len(pts)):
			s+=str(pts[j][0])+"\t"+str(pts[j][1])+"\n"
		open(f.replace(".npy","-unclutter-"+str(i)+".tsv"),'w').write(s)

	break;