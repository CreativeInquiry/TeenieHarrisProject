import cv2
import numpy as np

im = cv2.imread("../bin/data/icons/sort2.png", cv2.IMREAD_UNCHANGED)
im = cv2.cvtColor(im,cv2.COLOR_RGB2RGBA)
im[:,:,3] = im[:,:,0]
im[:,:,0] = 255
im[:,:,1] = 255
im[:,:,2] = 255
cv2.imwrite("../bin/data/icons/sort2.png",im)