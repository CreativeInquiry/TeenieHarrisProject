from glob import glob
import os
import json



files = [z for z in [[y if i==0 else int(y) for i,y in enumerate(x.split("\t")[1:])] for x in open("../bin/data/canonical_image_dimensions_1600.tsv",'r').read().split("\n")[1:]] if len(z)]
meta = []

for idx,[f,w,h] in enumerate(files):
	age_avg = 0
	age_median = 0
	age_bigface = 0
	age_min = 0
	age_max = 0
	area_bigface = 0
	pcarea_faces = 0
	n_faces = 0

	try:
		if not (idx % 10000):
			print(idx,'/',len(files))


		data_str = open("../bin/data/meta/ms_cog/msft_"+f+".json",'r').read()

		data = json.loads(data_str)
		
		n_faces = len(data['faces'])

		if n_faces:
			age_min = float('inf')

			for face in data['faces']:
				area = face['width']*face['height']
				pcarea_faces += area

				if (area > area_bigface):
					area_bigface = area
					age_bigface = face['age']
				
				age_avg += face['age']
				age_min = min(face['age'],age_min)
				age_max = max(face['age'],age_max)

			age_avg /= float(n_faces)

			age_median = sorted([x['age'] for x in data['faces']])[n_faces//2]

			pcarea_faces /= float(w*h) 

	except FileNotFoundError:
		print("cannot open file",f)

	meta.append( {'FILE':f,
				  'CANONICAL':idx,
				  'N_FACES':n_faces,
				  'AGE_AVG':age_avg,
				  'AGE_MIN':age_min,
				  'AGE_MAX':age_max,
				  'AGE_MEDIAN':age_median,
				  'AGE_BIGFACE':age_bigface,
				  'PCAREA_FACES':pcarea_faces,
				  'AREA_BIGFACE':area_bigface})

# print(meta)

def make_embedding(key='AGE_AVG'):
	global meta,files

	print(key)

	meta = sorted(meta, key=lambda x: x[key]);
	corder = [x['CANONICAL'] for x in meta]

	W = 277
	H = 214
	uw = 0.1

	s = ""
	for cidx in range(len(meta)):
		idx = corder.index(cidx);

		i = idx // W
		j = idx % W
		if meta[idx]['N_FACES'] == 0:
			i -= 10
		s += str((j-W//2)*uw) + "\t" + str((i-H//2)*uw)+"\n"

	return s

open("../bin/data/embeddings/sort/rect-sort-area_bigface.tsv", 'w').write(make_embedding(key='AREA_BIGFACE'))
open("../bin/data/embeddings/sort/rect-sort-n_faces.tsv", 'w').write(make_embedding(key='N_FACES'))
open("../bin/data/embeddings/sort/rect-sort-age_median.tsv", 'w').write(make_embedding(key='AGE_MEDIAN'))
open("../bin/data/embeddings/sort/rect-sort-age_avg.tsv", 'w').write(make_embedding(key='AGE_AVG'))
open("../bin/data/embeddings/sort/rect-sort-age_min.tsv", 'w').write(make_embedding(key='AGE_MIN'))
open("../bin/data/embeddings/sort/rect-sort-age_max.tsv", 'w').write(make_embedding(key='AGE_MAX'))
open("../bin/data/embeddings/sort/rect-sort-age_bigface.tsv", 'w').write(make_embedding(key='AGE_BIGFACE'))
open("../bin/data/embeddings/sort/rect-sort-pcarea_faces.tsv", 'w').write(make_embedding(key='PCAREA_FACES'))


	