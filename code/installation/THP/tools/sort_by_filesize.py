from glob import glob
import os
import json

import os

files = [z for z in [[y if i<2 else int(y) for i,y in enumerate(x.split("\t")[0:])] for x in open("../bin/data/canonical_image_dimensions_1600.tsv",'r').read().split("\n")[1:] if len(x)]]

meta = []
for i in range(len(files)):
	f = files[i]

	p = "../bin/data/images/full/"+f[0]+"/"+f[1]
	fsize =os.path.getsize(p)
	bpp = float(fsize)/float(f[2]*f[3])

	meta.append((i,fsize,bpp))
meta.sort(key=lambda x: x[2])

corder = [x[0] for x in meta]

W = 214
H = 277
uw = 0.1

s = ""
for cidx in range(len(meta)):
	idx = corder.index(cidx);
	i = idx // W
	j = idx % W
	s += str((j-W//2)*uw) + "\t" + str((i-H//2)*uw)+"\n"

# open("../bin/data/embeddings/sort/rect-sort-filesize.tsv",'w').write(s)
open("../bin/data/embeddings/sort/rect-sort-bytes_per_pix.tsv",'w').write(s)