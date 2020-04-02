from glob import glob
import os
import json
import math
import cv2
import numpy as np
from nms import nms

files = [z for z in [[y if i==0 else int(y) for i,y in enumerate(x.split("\t")[1:])] for x in open("../bin/data/canonical_image_dimensions_1600.tsv",'r').read().split("\n")[1:] if len(x)]]
meta = []

def rd(x):
	return str(round(x * 10000)/10000.0)

s = []
for idx,[f,W,H] in enumerate(files):
	l = []

	if not (idx % 1000):
		print(idx,'/',len(files))
	data_str = open("../bin/data/meta/json/"+f.split(".")[0]+".json",'r').read()
	data = json.loads(data_str)

	boxes = []
	if 'people' not in data:
		print('no people',idx)
		s.append('')
		continue

	if W>H:
		H=800*H/W
		W=800
	else:
		W=800*W/H
		H=800



	for peo in data['people']:
		versions = []
		servs = peo['faceData']
		for k in servs:
			v = [servs[k]['X'],servs[k]['Y'],servs[k]['W'],servs[k]['H']]
			if k == "OpenFace":
				versions = [v]
				break
			else:
				versions.append(v)
		box = [0,0,0,0]
		for v in versions:
			box[0]+=v[0]
			box[1]+=v[1]
			box[2]+=v[2]
			box[3]+=v[3]
		box[0]/=len(versions)
		box[1]/=len(versions)
		box[2]/=len(versions)
		box[3]/=len(versions)
		boxes.append(box)

	for f in boxes:
		# print([W,H,f['box']])
		x = f[0]/W
		y = f[1]/H
		w = f[2]/W
		h = f[3]/H
		l.append(rd(x)+"\t"+rd(y)+"\t"+rd(w)+"\t"+rd(h))

	s.append("\t".join(l))

# open("../bin/data/salient-boxes.tsv",'w').write("\n".join(s))
open("../bin/data/4-serv-face-boxes.tsv",'w').write("\n".join(s))