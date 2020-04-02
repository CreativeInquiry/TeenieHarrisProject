from glob import glob
import os
import json
import math
import cv2
import numpy as np
from nms import nms

#https://github.com/dmishin/tsp-solver
from tsp_solver.greedy_numpy import solve_tsp

files = [z for z in [[y if i==0 else int(y) for i,y in enumerate(x.split("\t")[1:])] for x in open("../bin/data/canonical_image_dimensions_1600.tsv",'r').read().split("\n")[1:] if len(x)]]
meta = []

sal = np.load("../bin/data/meta/saliency_images_32x32.npy")

def rd(x):
	return str(round(x * 1000)/1000.0)

s = []
for idx,[f,W,H] in enumerate(files):
	l = []


	if not (idx % 1000):
		print(idx,'/',len(files))
	data_str = open("../bin/data/meta/openface_json/"+f+".json",'r').read()
	data = json.loads(data_str)

	#ff = "G:\\THP\\TEENIE\\JPG640\\"+f.replace(".png",".jpeg")
	#fff = "E:\\TeenieHarrisProject\\of_v0.10.1_vs2017_release\\apps\\myApps\\THP\\bin\\data\\images\\512\\"+f
	#im = cv2.imread(ff,0)
	#im2 = cv2.imread(fff,0)


	boxes = [f['box'] for f in data['faces']]
	#boxes = []

	if (len(boxes)<=0):
		nmsim, argmaxes = nms(sal[idx]/np.max(sal[idx]))
		
		#cv2.imshow('',nmsim)
		#cv2.waitKey(10)

		d = 50
		for (x,y,r) in argmaxes:
			if (r < 0.2):
				continue
			ss = 0
			pw = 0
			ph = 0
			if (W > H):
				ss = H/32.0
				pw = (W-H)/2.0
			else:
				ss = W/32.0
				ph = (H-W)/2.0
			
			xf = (x+0.5)*ss+pw;
			yf = (y+0.5)*ss+ph;

			if xf < d: xf = d
			if xf > W-d: xf = W-d
			if yf < d: yf = d
			if yf > H-d: yf = H-d

			boxes.append([xf-d,yf-d,xf+d,yf+d])

	D = []
	for i in range(len(boxes)):
		d = []
		f = boxes[i]
		x0 = (f[0]+f[2])/2
		y0 = (f[1]+f[3])/2
		for j in range(i):
			g = boxes[j]
			x1 = (g[0]+g[2])/2
			y1 = (g[1]+g[3])/2

			d.append(math.sqrt((x0-x1)**2+((y0-y1)*4)**2))
		D.append(d)
	path = solve_tsp(D)
	

	faces = [boxes[p] for p in path]


	#cv2.imshow('',im)
	#cv2.waitKey(0)
	#cv2.imshow('',(im.astype('float32')/255.0)*(cv2.resize(sal[idx],(im.shape[1],im.shape[0])).astype('float32')/255.0))
	#cv2.waitKey(0)
	#cv2.imshow('',(im2.astype('float32')/255.0)*(cv2.resize(sal[idx],(im2.shape[1],im2.shape[0])).astype('float32')/255.0))
	#cv2.waitKey(0)


	for f in faces:
		# print([W,H,f['box']])
		x = f[0]/W
		y = f[1]/H
		w = (f[2]-f[0])/W
		h = (f[3]-f[1])/H
		l.append(rd(x)+"\t"+rd(y)+"\t"+rd(w)+"\t"+rd(h))

	s.append("\t".join(l))

# open("../bin/data/salient-boxes.tsv",'w').write("\n".join(s))
open("../bin/data/face-boxes.tsv",'w').write("\n".join(s))