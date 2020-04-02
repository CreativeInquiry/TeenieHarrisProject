import numpy as np
import cv2
im = cv2.imread("../bin/data/thumbs/thumbs_64x64.png",0)
im2 = np.zeros((256*64,256*64),np.uint8)
for i in range(0,im.shape[0]):
	x = (i % 256)*64
	y = (i // 256)*64
	im2[y:y+64,x:x+64] = im[i].reshape(64,64)
cv2.imwrite("../bin/data/thumbs/thumbs_texture_64x64.png",im2)