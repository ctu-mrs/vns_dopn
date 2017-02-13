import os
from string import Template
import subprocess
import re
from __builtin__ import exit
from shutil import copyfile

#for my PC
#SCRIPT_OUTPUT_FOLDER = "generatedScripts/"
#SCRIPT_START_FOLDER = "scriptStartFolder/"

#for metacenter
SCRIPT_OUTPUT_FOLDER = "/storage/praha1/home/penicrob/generatedScripts/"
SCRIPT_START_FOLDER = "/storage/praha1/home/penicrob/scriptStartFolder/"

PROBLEMS_PATH = "../"

if not os.path.exists(SCRIPT_OUTPUT_FOLDER):
    os.makedirs(SCRIPT_OUTPUT_FOLDER)

if not os.path.exists(SCRIPT_START_FOLDER):
    os.makedirs(SCRIPT_START_FOLDER)

scriptId = 0

print "remove all from folder SCRIPT_OUTPUT_FOLDER"
print os.listdir(SCRIPT_OUTPUT_FOLDER)
filelist = os.listdir(SCRIPT_OUTPUT_FOLDER)
for oldFilename in filelist:
    print("remove " + oldFilename)
    os.remove(SCRIPT_OUTPUT_FOLDER + oldFilename)
    
print "remove all from folder SCRIPT_START_FOLDER"
print os.listdir(SCRIPT_START_FOLDER)
filelist = os.listdir(SCRIPT_START_FOLDER)
for oldFilename in filelist:
    print("remove " + oldFilename)
    os.remove(SCRIPT_START_FOLDER + oldFilename)

# copy start script
START_SCRIPT_NAME = "startAllScriptsDiffNeighSampling.py"
copyfile(START_SCRIPT_NAME, SCRIPT_START_FOLDER + START_SCRIPT_NAME)


templateName = "./template.sh"
# templateName = "pok";


# radiueses = [0.0 , 0.3 , 0.5 , 0.9 , 1.1 , 1.3 ]
# radiueses = [ 0.3 , 0.5 , 0.9 ]
radiueses = [ 0.5 ]

# neigh_radiuses = [0.0 , 0.2 , 0.5 , 1.0 , 1.5 , 2.0]
# neigh_radiuses = [ 0.2 , 0.5 , 1.0 ]
neigh_radiuses = [ 0.5 ]

testIds = range(0, 4)
neigh_resolutions = [ 1 , 2 , 4 , 8 , 12 , 16 , 20 , 24 ]

RESOLUTION = 16
DEFAULT_RESOLUTION = 16
NEIGHBORHOOD_RESOLUTION = 16
MAX_COMP_TIME_MIN = 220
NUM_ITERS_UNIMPR = 5000
NUM_ITERS = 10000
RVNS = 1
dict = {'NAME':'noname', 'RESOLUTION':RESOLUTION, 'MAX_COMP_TIME_MIN':MAX_COMP_TIME_MIN, 'NUM_ITERS_UNIMPR': NUM_ITERS_UNIMPR , 'NUM_ITERS':NUM_ITERS, 'NEIGHBORHOOD_RESOLUTION':NEIGHBORHOOD_RESOLUTION, 'NEIGHBORHOOD_RADIUS': '0.3', 'RVNS':RVNS, 'RADIUS':'1.3', 'PROBLEM':'etc/tsiligrides_problem1/tsiligirides_problem_1_budget_20.txt', 'OUTPUT_FOLDER':'out'}

PROBLEM_FILES = {'P3':'etc/tsiligrides_problem3/tsiligirides_problem_3_budget_055.txt','64':'etc/chao_set64-diamond/chao_set_64_1_55.txt','66':'etc/chao_set66-square/chao_set_66_1_060.txt'}
#PROBLEM_FILES = {'64':'etc/chao_set64-diamond/chao_set_64_1_55.txt', '66':'etc/chao_set66-square/chao_set_66_1_060.txt'}


PROBLEM_NAMES = {'P3':'Tsiligirides Set 3', '64':'Chao diamond-shaped Set', '66':'Chao squared-shaped Set'}
#PROBLEM_NAMES = {'64':'Chao diamond-shaped Set', '66':'Chao squared-shaped Set'}

templateFile = open(templateName, "r") 
src = Template(templateFile.read()) 
templateFile.close()

# PROBLEMS_PATH = "/storage/praha1/home/penicrob/icra17dop/gacr16/h_dop/etc/"

num_created = 0
for NEIGHBORHOOD_RADIUS in neigh_radiuses:
    for RADIUS in radiueses:
        
        for problem_key in PROBLEM_NAMES.iterkeys():
            problem_name = PROBLEM_NAMES[problem_key]
            problem_with_path = PROBLEM_FILES[problem_key]
            
            for TEST_ID in testIds :
            
                for NEIGHBORHOOD_RESOLUTION in neigh_resolutions:       
                
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
                        num_created += 1
                        dict['PROBLEM_NAME'] = problem_name
                        dict['NEIGHBORHOOD_RADIUS'] = NEIGHBORHOOD_RADIUS
                        dict['NEIGHBORHOOD_RESOLUTION'] = NEIGHBORHOOD_RESOLUTION
                        dict['RADIUS'] = RADIUS
                        dict['OUTPUT_FOLDER'] = "result_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_nres_" + str(NEIGHBORHOOD_RESOLUTION) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID)
                        dict['PROBLEM'] = problem_with_path
                        dict['NAME'] = "dop_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_nres_" + str(NEIGHBORHOOD_RESOLUTION) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID)
                        scriptName = SCRIPT_OUTPUT_FOLDER + "start_r_" + str(RADIUS) + "_nr_" + str(NEIGHBORHOOD_RADIUS) + "_nres_" + str(NEIGHBORHOOD_RESOLUTION) + "_p_" + problem_key + "_b_" + budget + "_run_" + str(TEST_ID) + ".bash"
                        print "create " + scriptName
                        result = src.substitute(dict)
                        scriptFile = open(scriptName , "w")
                        scriptFile.write(result)
                        scriptFile.close()


print("we created " + str(num_created) + " files to run in " + SCRIPT_OUTPUT_FOLDER)

# subprocess.call("pwd")
# subprocess.call(["ls", "-l"])
