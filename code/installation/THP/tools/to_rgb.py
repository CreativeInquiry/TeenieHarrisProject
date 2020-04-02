import cv2
from glob import glob
files = glob("../bin/data/thumbs/thumbs*.*g")
for f in files:
	print(f)
	bname = f.split("\\")[-1].split(".")[0]
	im = cv2.imread(f)
	cv2.imwrite("../bin/data/thumbs/"+bname+"-rgb.png",im)
	break