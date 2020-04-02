import cv2
im = cv2.imread("thumbs_64x64.png",0)

h,w = im.shape
H = 10000
for i in range(0,h,H):
	print(i)
	cv2.imwrite("thumbs_64x64_"+str(i)+"-"+str(i+H)+".png",im[i:i+H])