import os
from string import Template
import subprocess
import re
from __builtin__ import exit
from shutil import copyfile
from sys import exit

REAL_RUN = False

#for my PC
#SCRIPT_OUTPUT_FOLDER = "../generatedScripts/"
#PROBLEMS_PATH = "../../"

#for metacenter
SCRIPT_OUTPUT_FOLDER = "/storage/praha1/home/penicrob/generatedScripts/"
PROBLEMS_PATH = "/storage/praha1/home/penicrob/gacr16/vns_dopn/"


#radiueses = [0.0 , 0.3 , 0.5 , 0.9 , 1.1 , 1.3 ]
radiueses = [ 0.3 , 0.5 , 0.9 ]

#neigh_radiuses = [0.0 , 0.2 , 0.5 , 1.0 , 1.5 , 2.0]
neigh_radiuses = [ 0.2 , 0.5 , 1.0]

testIds = range(0, 10)

RESOLUTION = 16
NEIGHBORHOOD_RESOLUTION = 16
MAX_COMP_TIME_MIN = 110
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 10000
RVNS = 1
dict = {'NAME':'noname', 'RESOLUTION':RESOLUTION, 'MAX_COMP_TIME_MIN':MAX_COMP_TIME_MIN, 'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR , 'NUM_ITERS':NUM_ITERS, 'NEIGHBORHOOD_RESOLUTION':NEIGHBORHOOD_RESOLUTION, 'NEIGHBORHOOD_RADIUS': '0.3', 'RVNS':RVNS, 'RADIUS':'1.3', 'PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt', 'OUTPUT_FOLDER':'out'}

PROBLEM_FOLDERS = {'P1':'etc/tsiligrides_problem1/', 'P2':'etc/tsiligrides_problem2/', 'P3':'etc/tsiligrides_problem3/', '64':'etc/chao_set64-diamond/', '66':'etc/chao_set66-square/'}

#PROBLEM_NAMES = {'P1':'Tsiligirides Set 1', 'P2':'Tsiligirides Set 2', 'P3':'Tsiligirides Set 3', '64':'Chao diamond-shaped Set', '66':'Chao squared-shaped Set'}
PROBLEM_NAMES = {'66':'Chao squared-shaped Set'}

num_created = 0
for NEIGHBORHOOD_RADIUS in neigh_radiuses:
    for RADIUS in radiueses:
        for TEST_ID in testIds :
            for problem_key in PROBLEM_NAMES.iterkeys():
                problem_path = PROBLEM_FOLDERS[problem_key]
                problem_name = PROBLEM_NAMES[problem_key]
                print problem_path
                filenames = next(os.walk(PROBLEMS_PATH + problem_path))[2]
                for problem_filename in filenames:
                    problem_with_path = problem_path + problem_filename
                    budget = 0;
                    with open(PROBLEMS_PATH + problem_with_path) as f:
                        for line in f:
                            s = re.search(r'\d+', line)
                            if s:
                                budget = s.group().zfill(3)
                                break
                        scriptName = SCRIPT_OUTPUT_FOLDER + "start_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID) + ".bash"
                        print("starting",scriptName)
                        import os.path
                        if os.path.exists(scriptName):
                            num_created += 1
                            print("run the "+scriptName)
                            if REAL_RUN:
                                subprocess.call(["qsub",scriptName])
                            else:
                                print("the real run would call qsub "+scriptName)
                        else:
                            print("file does not exists "+scriptName)
                            exit(1)
                            
                            
if not REAL_RUN:
    print("we would run "+str(num_created)+" from "+SCRIPT_OUTPUT_FOLDER)
else:
    print("we ran "+str(num_created)+" from "+SCRIPT_OUTPUT_FOLDER)
