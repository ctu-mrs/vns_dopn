#!/usr/bin/python3

import os
import subprocess

folderOut = "generatedScripts/"

scriptnames = next(os.walk(folderOut))[2]
print("test")
for script in scriptnames:
    fullname = folderOut+script
    #print(fullname)
    subprocess.call(["ls",fullname])
    subprocess.call(["qsub",fullname])
    
#subprocess.call(["ls", "-l",folderOut])
