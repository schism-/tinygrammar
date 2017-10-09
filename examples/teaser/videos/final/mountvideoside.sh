ffmpeg \
  -i 2kf.gif \
  -i 4kf.gif \
  -i 6kf.gif \
  -i 8kf.gif \
  -filter_complex '[0:v]pad=iw*4:ih[int];[int][1:v]overlay=W/4:0[int];[int][2:v]overlay=W/2:0[int];[int][3:v]overlay=0.75*W:0[vid]' \
  -map [vid] \
  -c:v libx264 \
  -crf 23 \
  -preset veryfast \
  output.mp4