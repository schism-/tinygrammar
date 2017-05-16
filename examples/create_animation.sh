#!/bin/bash
# Run application and creates GIF given SVG frames

#1: grammar
#2: svg
#3: output folder

if [ ! -d "$3/frames" ]; then
	mkdir -p $3/frames
else 
	rm -rf $3/frames
fi

./../Bin/Release/tinygrammar_time $1 $2 $3/frames $4

#echo "CREATING PNG FRAMES"
#mogrify -format png $3/frames/*.svg

#convert -density 20 -resize 500x500 $3/svgs/*.svg -set filename:base "%[base]" $3/pngs/"%[filename:base].png"
#echo "CREATING GIF"
#convert -delay 1/30 -loop 0 $3/frames/*.png $3/animated.gif
