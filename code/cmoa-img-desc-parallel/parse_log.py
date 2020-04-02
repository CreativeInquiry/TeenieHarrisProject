from glob import glob
logs1 = glob("log/*.log")
logs2 = glob("log_copy/*.log")
logs = [l if ("log/"+l.split("/")[1] not in logs1) else ("log/"+l.split("/")[1]) for l in logs2 ]
badones = []
for f in logs:
    badones += [(x.split(" ")[-1],x.split(" ")[1]) for x in open(f,'r').read().split("\n") if ("MANUAL CHECK" in x) or ("SHIT" in x)]
print(badones,len(badones))