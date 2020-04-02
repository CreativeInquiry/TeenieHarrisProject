import cv2
import numpy as np
import json

meta = json.loads(open("meta.json",'r').read())
meta_by_shade = sorted(meta,key=lambda x: x['COLOR_AVG']);


W = 214
H = 277
uw = 0.1

def do_pic(im):
	l = np.reshape(im,(W*H))
	L = []
	for i in range(len(l)):
		L.append([i,l[i]])
	L.sort(key=lambda x:x[1])

	out = [None for x in meta_by_shade]

	#test = np.zeros((H,W),np.uint8)

	for idx in range(len(L)):
		ij,lum = L[idx]
		i = ij // W
		j = ij % W

		x,y = (j-W//2)*uw,  (i-H//2)*uw

		out[meta_by_shade[idx]['CANONICAL']]=[x,y]

		#test[i,j] = lum

	# cv2.imshow('',test)
	# cv2.waitKey(0)

	return out

def write_it(out_path, out):
	s = "\n".join([str(x[0])+"\t"+str(x[1]) for x in out])
	open(out_path, 'w').write(s)

for i in range(len(meta)):
	file = meta[i]['FILE']
	a = meta[i]['AREA_BIGFACE']
	b = a > 40000
	print(file,a,b)
	if not b:
		continue
	im = cv2.imread("SQUARES/512/"+file,0)
	im = cv2.resize(im, (277,277))
	im = im[:,32:246]
	out = do_pic(im)
	write_it("mosaic/"+file+".tsv", out)
