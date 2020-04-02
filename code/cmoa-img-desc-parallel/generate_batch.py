import math

boxes = sorted(list(set([x.split(".")[0].split("/")[0] for x in str(open("canonical_filename_order.txt",'r').read()).split("\n") if len(x)])))
boxes = list(reversed(boxes))
nps = 250
n_scripts = int(math.ceil(float(len(boxes))/float(nps)))
for i in range(n_scripts):
	resp = boxes[i*nps:i*nps+nps]
	script = 'boxes=("'+('" "').join(resp)+'")\nfor t in ${boxes[@]}; do\n  python3 scraper.py $t > log/$t.log\ndone\n'
	open("batch/"+str(i).zfill(3)+".sh",'w').write(script)
