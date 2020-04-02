import cv2
import numpy as np
from glob import glob
#59278
import sys
print(sys.maxsize)
W = 256
n = 256
path = "F:/LINGDONG/Teenie_Harris_JPG640/Teenie_Harris_JPG640/"
files = [path+x.split("/")[-1].split(".")[0]+".jpeg" for x in open("../bin/data/canonical_filename_order.txt",'r').read().split("\n")]

thumbs = np.zeros((n*W,n*W),np.uint8)

for f in files:
	print(f)

	im = cv2.cvtColor(cv2.imread(f),cv2.COLOR_RGB2GRAY)
	im = im[20:-20,20:-20]
	h,w = im.shape
	if h > w:
		im = im[(h-w)//2:(h-w)//2+w,:]
	else:
		im = im[:,(w-h)//2:(w-h)//2+h]
	im = cv2.resize(im,(W,W))

	x = (i % n)*W
	y = (i // n)*W
	thumbs[y:y+W,x:x+W] = im


thumbs = np.array(thumbs)
cv2.imwrite("thumbs_texture_"+W+"x"+W+".png",thumbs)