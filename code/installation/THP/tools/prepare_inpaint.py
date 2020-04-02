from glob import glob
import os
import cv2
import numpy as np

data_root = "/Users/admin/Downloads/TEENIE/"

files = glob(data_root+"JPG640/*.jpeg")


for i in range(0,len(files)):
	if i == 0:
		continue
	try:
		basename = files[i].split("/")[-1].split(".")[0]
		print(basename)
		im = cv2.imread(files[i])
		mask = cv2.imread(data_root+"detectron_rendered_contours_1600px/"+basename+".png")
		
		mask = cv2.resize(mask,(im.shape[1],im.shape[0]))

		mask = cv2.cvtColor(mask,cv2.COLOR_BGR2GRAY)
		im = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)

		im = cv2.inpaint(im,mask,2,cv2.INPAINT_NS)
		
		# patch = np.ones((im.shape[0],im.shape[1]),dtype=np.uint8)*255

		# patch = cv2.cvtColor(patch,cv2.COLOR_GRAY2BGR)

		# dst = cv2.seamlessClone(patch,im,mask,(im.shape[1]/2,im.shape[0]/2),cv2.NORMAL_CLONE)

		# cv2.imshow("",dst)
		# cv2.waitKey(0)

		# mask = cv2.erode(mask,cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5)))

		# mask = cv2.GaussianBlur(mask,(7,7),0)

		# cv2.imshow("",mask)
		# cv2.waitKey(0)

		# im = np.concatenate((im, mask), axis=2)[:,:,:4]

		# im = cv2.multiply(im,1-mask)+mask*[0,1,0]


		# im2 = cv2.multiply(im,1-mask)

		# im2 = cv2.GaussianBlur(im2,(501,501),0)

		# im2 = cv2.multiply(im2, mask)

		# im = cv2.multiply(im,1-mask) + im2

		cv2.imwrite("out2/"+basename+".png",im*255)
	except Exception as e:
		print(e)
		print("^ERROR!!!!!!!!!!!!!!!!!!!!")