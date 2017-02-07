import os
from string import Template
import subprocess
import re
from __builtin__ import exit
from shutil import copyfile

#SCRIPT_OUTPUT_FOLDER = "generatedScripts/"
SCRIPT_OUTPUT_FOLDER = "/storage/praha1/home/penicrob/generatedScripts/"
PROBLEMS_PATH = "../etc/"

if not os.path.exists(SCRIPT_OUTPUT_FOLDER):
    os.makedirs(SCRIPT_OUTPUT_FOLDER)

#copy start script
START_SCRIPT_NAME = "startAllScripts.py"
copyfile(START_SCRIPT_NAME, SCRIPT_OUTPUT_FOLDER+START_SCRIPT_NAME)

scriptId = 0

print "remove all from folder SCRIPT_OUTPUT_FOLDER"
print os.listdir(SCRIPT_OUTPUT_FOLDER)
filelist = os.listdir(SCRIPT_OUTPUT_FOLDER)
for oldFilename in filelist:
    print("remove " + oldFilename)
    os.remove(SCRIPT_OUTPUT_FOLDER + oldFilename)



templateName = "./template.sh"
# templateName = "pok";


radiueses = [0.0 , 0.3 , 0.5 , 0.7 , 0.9 , 1.1 , 1.3 ]
# radiueses = [0.0]

neigh_radiuses = [0.0 , 0.2 , 0.5 , 0.7 , 1.0 , 1.2 , 1.5 , 1.7 , 2.0]
testIds = range(0, 1)

RESOLUTION = 16
NEIGHBORHOOD_RESOLUTION = 16
MAX_COMP_TIME_MIN = 110
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 10000
RVNS = 1
dict = {'NAME':'noname', 'RESOLUTION':RESOLUTION, 'MAX_COMP_TIME_MIN':MAX_COMP_TIME_MIN, 'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR , 'NUM_ITERS':NUM_ITERS, 'NEIGHBORHOOD_RESOLUTION':NEIGHBORHOOD_RESOLUTION, 'NEIGHBORHOOD_RADIUS': '0.3', 'RVNS':RVNS, 'RADIUS':'1.3', 'PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt', 'OUTPUT_FOLDER':'out'}
PROBLEM_NAMES = {'P1':'Tsiligirides Set 1', 'P2':'Tsiligirides Set 2', 'P3':'Tsiligirides Set 3', '64':'Chao diamond-shaped Set', '66':'Chao squared-shaped Set'}
PROBLEM_FOLDERS = {'P1':'tsiligrides_problem1/', 'P2':'tsiligrides_problem2/', 'P3':'tsiligrides_problem3/', '64':'chao_set64-diamond/', '66':'chao_set66-square/'}

templateFile = open(templateName, "r") 
src = Template(templateFile.read()) 
templateFile.close()

# PROBLEMS_PATH = "/storage/praha1/home/penicrob/icra17dop/gacr16/h_dop/etc/"


for NEIGHBORHOOD_RADIUS in neigh_radiuses:
    for RADIUS in radiueses:
        for TEST_ID in testIds :
            for problem_key in PROBLEM_FOLDERS.iterkeys():
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
                    # print budget
                    # templateFile = open("../"+problem_with_path, "r")
                    # print problem_with_path
                        dict['PROBLEM_NAME'] = problem_name
                        dict['NEIGHBORHOOD_RADIUS'] = NEIGHBORHOOD_RADIUS
                        dict['RADIUS'] = RADIUS
                        dict['OUTPUT_FOLDER'] = "result_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID)
                        dict['PROBLEM'] = problem_with_path
                        dict['NAME'] = "dop_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID)
                        scriptName = SCRIPT_OUTPUT_FOLDER + "start_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID) + ".bash"
                        print "create " + scriptName
                        result = src.substitute(dict)
                        scriptFile = open(scriptName , "w")
                        scriptFile.write(result)
                        scriptFile.close()


# subprocess.call("pwd")
# subprocess.call(["ls", "-l"])
