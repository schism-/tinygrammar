#!/bin/bash

s=""
for d in */ ; do
    s="$(basename $d)"
    if [ ! -d $s/ ]; then
    	echo "Folder not found!"
	else 
		rm -rf $s/frames
	fi
done