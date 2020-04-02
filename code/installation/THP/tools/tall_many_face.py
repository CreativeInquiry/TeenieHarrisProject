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
sortable = []
for idx,[f,W,H] in enumerate(files):
	l = []

	if not (idx % 1000):
		print(idx,'/',len(files))
	data_str = open("../bin/data/meta/json/"+f.split(".")[0]+".json",'r').read()
	data = json.loads(data_str)

	boxes = []
	if (len(data['people']) >= 3):
		sortable.append([H/W,f,idx])

# open("../bin/data/salient-boxes.tsv",'w').write("\n".join(s))
# open("../bin/data/4-serv-face-boxes.tsv",'w').write("\n".join(s))

sortable.sort()
sortable = list(reversed(sortable))
print(sortable[0:20])