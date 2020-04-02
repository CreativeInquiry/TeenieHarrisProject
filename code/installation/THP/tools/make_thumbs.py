import cv2
import numpy as np
from glob import glob
#59278
thumbs = []
W = 64
path = "/Volumes/GOLAN/LINGDONG/Teenie_Harris_JPG640/Teenie_Harris_JPG640/"
files = [path+x.split("/")[-1].split(".")[0]+".jpeg" for x in open("canonical_filename_order.txt",'r').read().split("\n")]

for f in files:
	print(f)
	try:
		im = cv2.cvtColor(cv2.imread(f),cv2.COLOR_RGB2GRAY)
		im = im[20:-20,20:-20]
		h,w = im.shape
		if h > w:
			im = im[(h-w)//2:(h-w)//2+w,:]
		else:
			im = im[:,(w-h)//2:(w-h)//2+h]
		im = cv2.resize(im,(W,W))

		# cv2.imshow("",im)
		# cv2.waitKey(0)

		thumbs.append(im.reshape(W*W))
	except:
		pass

thumbs = np.array(thumbs)
cv2.imwrite("thumbs_64x64.png",thumbs)