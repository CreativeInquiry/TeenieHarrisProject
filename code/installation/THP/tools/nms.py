import cv2
import numpy as np
import json

def nms(im):
	im2 = np.zeros_like(im);
	def supress(y,x,d):
		for i in range(max(0,y-d),min(y+d,im.shape[0])):
			for j in range(max(0,x-d),min(x+d,im.shape[1])):
				if (i == y and j == x):
					continue
				if im[y,x]<im[i,j]:
					return 0
				elif im[y,x]==im[i,j]:
					if (y > i):
						return 0
					elif (y == i):
						if (x > j):
							return 0
		return im[y,x]

	argmaxes = []
	for i in range(im.shape[0]):
		for j in range(im.shape[1]):
			r=supress(i,j,3)
			if (r > 0):
				argmaxes.append([j,i,r])
				im2[i,j]=r

	return im2,argmaxes



if __name__ == "__main__":
	ims = np.load("../bin/data/meta/saliency_images_32x32.npy")

	s = [];

	for i in range(0,len(ims)):
		if i % 1000 == 0:
			print(i,'/',len(ims))
		im = ims[i]
		im2, argmaxes = nms(im)
		ims[i]=im2

		for a in argmaxes:
			s.append({'x':int(a[0]),'y':int(a[1]),'val':int(a[2])})

		# cv2.imshow('',cv2.resize(np.hstack([im,im2]),(0,0),fx=10,fy=10,interpolation=cv2.INTER_NEAREST))
		#cv2.waitKey(0)

	open("saliency_nms_32x32.json",'w').write(json.dumps(s))

	np.save("saliency_nms_32x32.npy",ims)