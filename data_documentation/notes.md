```
ssh myaccount@teenie.cfa.cmu.edu
cd /home/myaccount/Documents/TeenieHarrisProject
for f in */*; do cp $f ../json; echo $f; done
zip -r destination.zip sourcedir/
gsutil cp sourceFile.foo gs://teenieharris/dstdir
```