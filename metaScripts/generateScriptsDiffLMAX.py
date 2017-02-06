#!/usr/bin/python3

import os
from string import Template
import subprocess
import re


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
testIds = range(0,10)
LMAXs= [ 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 ]

RESOLUTION = 16;
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 10000
RVNS = 1
params = {'NAME':'noname','RESOLUTION':RESOLUTION,'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR ,'NUM_ITERS':NUM_ITERS,'RVNS':RVNS,'RADIUS':'1.3','PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt','OUTPUT_FOLDER':'out'}
PROBLEM_NAMES = {'P1':'Tsiligirides Set 1','P2':'Tsiligirides Set 2','P3':'Tsiligirides Set 3','64':'Chao diamond-shaped Set','66':'Chao squared-shaped Set'}
PROBLEM_FILES = {'P3':'etc/tsiligrides_problem3/tsiligirides_problem_3_budget_055.txt','64':'etc/chao_set64-diamond/chao_set_64_1_55.txt','66':'etc/chao_set66-square/chao_set_66_1_065.txt'}

templateFile = open(templateName, "r")
src = Template( templateFile.read() )
templateFile.close()

#H_DOP_PATH = "/storage/praha1/home/penicrob/icra17dop/gacr16/h_dop/"
H_DOP_PATH = "../"

for RADIUS in radiueses:
    if RADIUS == 0:
        resolution=1
    else :
        resolution=16
    for TEST_ID in testIds :
        for problem_key in PROBLEM_FILES.keys():
            problem_name=PROBLEM_NAMES[problem_key]
            priblem_with_path=PROBLEM_FILES[problem_key]
            budget = 0;
            with open(H_DOP_PATH+priblem_with_path) as f:
                for line in f:
                   s=re.search(r'\d+',line)
                   if s:
                       budget = s.group().zfill(3)
                       break
                for max_l in LMAXs:
                    params['RESOLUTION']=resolution
                    params['PROBLEM_NAME']=problem_name
                    params['RADIUS']=RADIUS
                    params['MAX_L']=max_l
                    params['OUTPUT_FOLDER']="result_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_maxl_"+("%02d" % (max_l,))+"_run_"+("%02d" % (TEST_ID,))
                    params['PROBLEM']=priblem_with_path
                    params['NAME']="dop_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_maxl_"+("%02d" % (max_l,))+"_run_"+("%02d" % (TEST_ID,))
                    scriptName = folderOut+"start_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+budget+'_res_'+str(resolution)+"_maxl_"+("%02d" % (max_l,))+"_run_"+("%02d" % (TEST_ID,))+".bash"
                    print("create "+scriptName)
                    result = src.substitute(params)
                    scriptFile = open(scriptName ,"w")
                    scriptFile.write(result)
                    scriptFile.close()

