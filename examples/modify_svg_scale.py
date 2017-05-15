from tempfile import mkstemp
from shutil import move
from os import remove, close
import os
import sys
from xml.dom import minidom

argv = sys.argv
directory = argv[1]
scalefactor = argv[2]

pattern = "<g transform=\"scale(3.000000,3.000000)\">"
subst = "<g transform=\"scale("+scalefactor+","+scalefactor+")\">"

print subst

def replace(file_path, pattern, subst):
    #Create temp file
    fh, abs_path = mkstemp()
    with open(abs_path,'w') as new_file:
        with open(file_path) as old_file:
            for line in old_file:
                new_file.write(line.replace(pattern, subst))
    close(fh)
    #Remove original file
    remove(file_path)
    #Move new file
    move(abs_path, file_path)


for filename in os.listdir(directory):
    if filename.endswith(".svg"):
        replace(directory+"/"+filename,pattern,subst)