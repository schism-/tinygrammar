#!/bin/bash
# Create GIF given SVG frames

./Bin/Release/tinygrammar_time $1 $2
convert -density 40 -resize 500x500 svg/*.svg -set filename:base "%[base]" png/"%[filename:base].png"
convert -delay 1/30 -loop 0 png/*.png animated.gif
