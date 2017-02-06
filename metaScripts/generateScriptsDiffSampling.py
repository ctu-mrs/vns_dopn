#!/usr/bin/python3

import os
from string import Template
import subprocess
import re
from __builtin__ import exit


folderOut = "generatedScripts/"
scriptId = 0
#subprocess.call(["rm", folderOut+"*"])

print("remove all from folder folderOut")
print(os.listdir(folderOut))
filelist = os.listdir(folderOut)
for oldFilename in filelist:
    print("remove "+oldFilename)
    os.remove(folderOut+oldFilename)



templateName = "./template.sh"
#templateName = "pok";


#radiueses = [0.0 , 0.3 , 0.5 , 0.7 , 0.9 , 1.1 , 1.3 ]
radiueses = [1.3]
testIds = range(0,20)
resolutions= [ 1 , 2, 3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11,12,13,14,15,16,20,24,28,32]

RESOLUTION = 16;
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 10000
RVNS = 1
dict = {'NAME':'noname','RESOLUTION':RESOLUTION,'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR ,'NUM_ITERS':NUM_ITERS,'RVNS':RVNS,'RADIUS':'1.3','PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt','OUTPUT_FOLDER':'out'}
PROBLEM_NAMES = {'P1':'Tsiligirides Set 1','P2':'Tsiligirides Set 2','P3':'Tsiligirides Set 3','64':'Chao diamond-shaped Set','66':'Chao squared-shaped Set'}
PROBLEM_FILES = {'P3':'etc/tsiligrides_problem3/tsiligirides_problem_3_budget_055.txt','64':'etc/chao_set64-diamond/chao_set_64_1_55.txt','66':'etc/chao_set66-square/chao_set_66_1_065.txt'}

templateFile = open(templateName, "r")
src = Template( templateFile.read() )
templateFile.close()

#H_DOP_PATH = "/storage/praha1/home/penicrob/icra17dop/gacr16/h_dop/"
H_DOP_PATH = "../"

for RADIUS in radiueses:
    if RADIUS == 0:
        dict['RESOLUTION']=1
    else :
        dict['RESOLUTION']=16
    for TEST_ID in testIds :
        for problem_key in PROBLEM_FILES.iterkeys():
            problem_name=PROBLEM_NAMES[problem_key]
            priblem_with_path=PROBLEM_FILES[problem_key]
            budget = 0;
            with open(H_DOP_PATH+priblem_with_path) as f:
                for line in f:
                   s=re.search(r'\d+',line)
                   if s:
                       budget = s.group().zfill(3)
                       break
                #print budget
                #templateFile = open("../"+priblem_with_path, "r")
                #print priblem_with_path
                for resolution in resolutions:
                    dict['RESOLUTION']=resolution
                    dict['PROBLEM_NAME']=problem_name
                    dict['RADIUS']=RADIUS
                    dict['OUTPUT_FOLDER']="result_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_run_"+str(TEST_ID)
                    dict['PROBLEM']=priblem_with_path
		    dict['NAME']="dop_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_run_"+str(TEST_ID)
		    scriptName = folderOut+"start_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_run_"+str(TEST_ID)+".bash"
                    print("create "+scriptName)
                    result = src.substitute(dict)
                    scriptFile = open(scriptName ,"w")
                    scriptFile.write(result)
                    scriptFile.close()


#subprocess.call("pwd")
#subprocess.call(["ls", "-l"])
