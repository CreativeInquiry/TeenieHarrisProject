from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options
import time
import os
import sys
from glob import glob
import re
import json
import random
import shutil
import re
import codecs

dones = [x for x in open("done.txt",'r').read().split("\n") if len(x)]

correct = json.loads(open("bad-corrected.json",'r').read())
# print(correct)

box = sys.argv[1]
print("DOING BOX: ",box)

chrome_options = Options()
# chrome_options.add_argument("--headless")
chrome_options.add_argument("safebrowsing-disable-extension-blacklist")
chrome_options.add_argument("--safebrowsing-disable-download-protection")
chrome_options.add_experimental_option("prefs", {'safebrowsing.enabled': 'false'})

filenames = [x.split(".")[0].split("/") for x in str(open("canonical_filename_order.txt",'r').read()).split("\n") if len(x)]
filenames = [x[1] for x in filenames if x[1] not in dones]

# filenames = [x for x in correct if correct[x] != "NO"]
print(filenames)
print("NUM STUFF IN BOX: ",len(filenames))
#filenames = [x.split("/")[1].split(".")[0] for x in str(open("canonical_filename_order.txt",'r').read()).split("\n") if len(x)]

def init_driver(path=os.path.join(os.getcwd(),"chromedriver")):
    driver = webdriver.Chrome(chrome_options=chrome_options, executable_path=path)
    return driver

def parse_info_html(html):
    url = html.split('href="')[1].split('">')[0]
    creator = html.split('creator-link\">')[1].split('</a>')[0]
    date = html.split('Date:</dt><dd class="search-result__value">')[1].split("</dd>")[0]
    desc = html.split('">')[2].split('</a>')[0]
    return url,desc,creator,date

def parse_accession_number(html):
    return html.split('Accession number:')[1].split('object__attributes-value">')[1].split('</dd>')[0]

driver = init_driver();
time.sleep(3);

for idx,fname in enumerate(filenames):

    print("now processing ",fname)
    entry = ("no description","no date","no accession number","no object id")

    try:
        driver.get("https://collection.cmoa.org/?q="+fname)

        search_results = []
        trials = 0

        while len(search_results) == 0:
            time.sleep(3)
            if (trials > 5):
                print("give up")
                break
            print("trial ",trials)
            search_results = driver.find_elements_by_class_name("search-result__info")
            trials += 1

        cands = []
        for x in search_results:

            html = x.get_attribute('innerHTML')
            iurl,desc,creator,date = parse_info_html(html)
            print(iurl,desc,creator,date)

            if (fname in correct):

                if correct[fname].split("/")[-1] != iurl.split("/")[-1]:
                    print("SKIPPING BECAUSE OF MANUAL LABEL", fname,correct[fname].split("/")[-1], iurl.split("/")[-1])
                    continue

            if (u"Teenie" in creator):
                driver.get("https://collection.cmoa.org"+iurl);
                time.sleep(2)
                obj = driver.find_elements_by_class_name("object")[1].get_attribute('innerHTML')
                # print(obj)
                acc = parse_accession_number(obj)
                print(acc)
            
                cands.append((desc,date,acc,iurl.split("/")[-1]))
                break
        if (len(cands) > 1):
            entry = cands[0]
            print("WARNING!!!!!! MULIPLE POSSIBLE RESULTS FOUND!!! MANUAL CHECK!!!", fname)
        elif (len(cands) == 0):
            print("WARNING!!!!!! NO RELAVENT RESULT FOUND!!! MANUAL CHECK!!!", fname)
        else:
            entry = cands[0]
        print("ENTRY:",fname,entry)

    except:
        print("SHIT!!!! DONT KNOW WHAT WENT WRONG",fname)
        print(sys.exc_info())

    codecs.open("out/"+box+".txt",'a+',encoding='utf8').write(fname+"\t"+entry[0]+"\t"+entry[1]+"\t"+entry[2]+"\t"+entry[3]+"\n")




