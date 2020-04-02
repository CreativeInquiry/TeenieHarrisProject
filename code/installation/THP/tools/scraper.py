import urllib.request
import re

x = str(urllib.request.urlopen('https://collection.cmoa.org/?q=6977').read())
results = re.findall(r'objects/',x)
print(results)