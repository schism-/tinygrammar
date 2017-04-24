#!/bin/bash
# Run application and creates GIF given SVG frames

#1: grammar
#2: svg
#3: output folder

if [ ! -d "$3/svgs" ]; then
	mkdir -p $3/svgs
else 
	rm -rf $3/svgs/*.svg
fi	

if [ ! -d "$3/pngs" ]; then
	mkdir -p $3/pngs
else 
	rm -rf $3/pngs/*.png
fi	

START_TIME=$SECONDS
./../Bin/Release/tinygrammar_time $1 $2 $3/svgs
echo "Execution ended in : $(($SECONDS - $START_TIME)) seconds!"

convert -density 40 -resize 500x500 $3/svgs/*.svg -set filename:base "%[base]" $3/pngs/"%[filename:base].png"

convert -delay 1/30 -loop 0 $3/pngs/*.png $3/animated.gif
