import numpy as np
import cv2

im = np.zeros((32,24,1),np.uint8)

im[8:24,8:16]=255

im = cv2.GaussianBlur(im, (9,9), cv2.BORDER_DEFAULT);
im = cv2.resize(im,(192,256))


im2 = np.zeros((256,192,4),np.uint8)
im2[:,:,3]=im

cv2.imwrite("../bin/data/icons/shadow.png",im2)