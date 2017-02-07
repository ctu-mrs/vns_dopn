#!/usr/bin/python3

import os
import subprocess

SCRIPT_OUTPUT_FOLDER = "generatedScripts/"

scriptnames = next(os.walk(SCRIPT_OUTPUT_FOLDER))[2]
print("test")
for script in scriptnames:
    fullname = SCRIPT_OUTPUT_FOLDER+script
    #print(fullname)
    subprocess.call(["ls",fullname])
    subprocess.call(["qsub",fullname])
    
#subprocess.call(["ls", "-l",folderOut])
