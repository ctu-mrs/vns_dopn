import os

#RESULT_PATH = '/storage/praha1/home/penicrob/icra17dop/results/'
#RESULT_PATH =  '../data/resultsmetaDiffRes/'
#RESULT_PATH = '../data/resultsDTSP/'
#RESULT_PATH = '../data/resultsDiffLMAXAdditional/'
RESULT_PATH = '../data/results_dopn_66_nr0.5_r0.5/'
#OUTPUT = '.log'
#OUTPUT = 'resultsDiffLMAXAdditional.log'
OUTPUT = '../data/results_dopn_66_nr0.5_r0.5/resultsAll.log'

with open(OUTPUT, "w")  as logAll:
    results = next(os.walk(RESULT_PATH))[1]
    for result_dir in results:
        print(result_dir)
        result_full_dir = RESULT_PATH+result_dir
        result_log = result_full_dir+'/results.log'
        #for problem_key in PROBLEM_NAMES.iterkeys():
        #if problem_key in result_log:
        #print(problem_key)
        #print(problem_key + "-" + PROBLEM_NAMES[problem_key])
        if (os.path.exists(result_log)):
            with open(result_log) as f:
                for line in f:
                    #replacedLine = line.replace('noName', PROBLEM_NAMES[problem_key])
                    #print(replacedLine)
                    logAll.write(line)
        else:
            print('file does not exists: '+result_log)
