import os
from string import Template
import subprocess
import re
from __builtin__ import exit


folderOut = "generatedScripts/"
scriptId = 0
#subprocess.call(["rm", folderOut+"*"])

print "remove all from folder folderOut"
print os.listdir(folderOut)
filelist = os.listdir(folderOut)
for oldFilename in filelist:
    print("remove "+oldFilename)
    os.remove(folderOut+oldFilename)



templateName = "./template.sh"
#templateName = "pok";

print('run')
#radiueses = [0.0 , 0.3 , 0.5 , 0.7 , 0.9 , 1.1 , 1.3 ]
radiueses = [1.3]
testIds = range(0,10)

RESOLUTION = 16;
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 15000
RVNS = 1
dict = {'NAME':'noname','RESOLUTION':RESOLUTION,'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR ,'NUM_ITERS':NUM_ITERS,'RVNS':RVNS,'RADIUS':'1.3','PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt','OUTPUT_FOLDER':'out'}
PROBLEM_NAMES = {'P1':'Tsiligirides Set 1','P2':'Tsiligirides Set 2','P3':'Tsiligirides Set 3','64':'Chao diamond-shaped Set','66':'Chao squared-shaped Set'}
PROBLEM_FOLDERS = {'P1':'problemsDTSP/1/','P2':'problemsDTSP/2/','P3':'problemsDTSP/3/','64':'problemsDTSP/64/','66':'problemsDTSP/66/'}


templateFile = open(templateName, "r")
src = Template( templateFile.read() )
templateFile.close()

#H_DOP_PATH = "/storage/praha1/home/penicrob/icra17dop/gacr16/h_dop/"
H_DOP_PATH = "./"

for RADIUS in radiueses:
    print RADIUS
    if RADIUS == 0:
        dict['RESOLUTION']=1
    else :
        dict['RESOLUTION']=16
    for TEST_ID in testIds :
        print TEST_ID
        for problem_key in PROBLEM_FOLDERS.iterkeys():
            print problem_key
            problem_path=PROBLEM_FOLDERS[problem_key]
            problem_name=PROBLEM_NAMES[problem_key]
            print problem_path
            print problem_path
            filenames = next(os.walk(H_DOP_PATH+problem_path))[2]
            for problem_filename in filenames:
                priblem_with_path=problem_path+problem_filename
                print(priblem_with_path)
                budget = 0;
                with open(H_DOP_PATH+priblem_with_path) as f:
                    for line in f:
                        if re.match("^\d+?\.\d+?$",  line.split(" ", 1)[0]):                         
                            budget = float(line.split(" ", 1)[0])
                            break
                #print budget
                #templateFile = open("../"+priblem_with_path, "r")
                #print priblem_with_path
                dict['PROBLEM_NAME']=problem_name
                dict['RADIUS']=RADIUS
                dict['OUTPUT_FOLDER']="result_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+str(budget)+"_run_"+str(TEST_ID)
                dict['PROBLEM']=priblem_with_path
                dict['NAME']="dop_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+str(budget)+"_run_"+str(TEST_ID)
                scriptName = folderOut+"start_r_"+str(RADIUS)+"_p_"+problem_key+"_b_"+str(budget)+"_run_"+str(TEST_ID)+".bash"
                print "create "+scriptName
                result = src.substitute(dict)
                scriptFile = open(scriptName ,"w")
                scriptFile.write(result)
                scriptFile.close()


#subprocess.call("pwd")
#subprocess.call(["ls", "-l"])
