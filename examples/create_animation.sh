#!/bin/bash
# Run application and creates GIF given SVG frames

#1: grammar
#2: svg

./../Bin/Release/tinygrammar_time $1 $2
convert -density 40 -resize 500x500 ../results/svg/*.svg -set filename:base "%[base]" ../results/png/"%[filename:base].png"
convert -delay 1/30 -loop 0 ../results/png/*.png ../results/$3.gif
