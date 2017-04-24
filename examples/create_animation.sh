#!/bin/bash
# Run application and creates GIF given SVG frames

#1: grammar
#2: svg

./../Bin/Release/tinygrammar_time $1 $2 $3
convert -density 40 -resize 500x500 $4/svgs/*.svg -set filename:base "%[base]" $4/pngs/"%[filename:base].png"
convert -delay 1/30 -loop 0 $4/pngs/*.png $4/$4.gif
