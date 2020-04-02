import cv2
import numpy as np
from glob import glob
# npy = np.load("photos_images_32x32.npy")
# img = npy.reshape(npy.shape[0],32*32)
# cv2.imwrite("img.png",img)

# files = glob("../bin/data/embeddings/assignment/*vgg*.npy")
# for f in files:
# 	print(f)
# 	npy = np.load(f)
# 	s = ""
# 	print(npy.shape[0])
# 	for i in range(npy.shape[0]):
# 		s+=str(-(0.5-npy[i][0])*20)+"\t"+str((0.5-npy[i][1])*20)+"\n"
# 	open(f.replace(".npy",".tsv"),'w').write(s)

fact1 = 214
fact2 = 277

# files = glob("../bin/data/embeddings/assignment/*.npy")
# for f in files:
# 	print(f)
# 	npy = np.load(f)
# 	s = ""
# 	print(npy.shape[0])
# 	for i in range(npy.shape[0]):
# 		s+=str((0.5-npy[i][0])*20*fact2/fact1)+"\t"+str((0.5-npy[i][1])*20)+"\n"
# 	open(f.replace(".npy",".tsv"),'w').write(s)


# 59278 = 2*107*277 = 214*277
# npy = np.load("/Users/admin/Downloads/vgg-features/colors.npy")
# img = npy.reshape(214,277,3)
# cv2.imwrite("img.png",img)

s = ""
for i in range(0,277):
	for j in range(0,214):
		s += str((j-214//2)/10.0) + "\t" + str((i-277//2)/10.0)+"\n"
open("../bin/data/rect-v.tsv",'w').write(s)