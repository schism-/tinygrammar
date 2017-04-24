#!/bin/bash

s=""
for d in */ ; do
    s="$(basename $d)"
    if [ ! -f $s/$s.svg ]; then
    	echo "File not found!"
	else 
		./create_animation.sh $s/$s.json $s/$s.svg $s/svgs/ $s
	fi
done