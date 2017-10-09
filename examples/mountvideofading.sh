#!/bin/bash

ffmpeg \
-loop 1 -t 3 -i derivation1.png \
-i 1.gif \
-i 1.gif \
-loop 1 -t 3 -i derivation2.png \
-i 2.gif \
-i 2.gif \
-loop 1 -t 3 -i derivation3.png \
-i 3.gif \
-i 3.gif \
-loop 1 -t 3 -i derivation4.png \
-i 4.gif \
-i 4.gif \
-filter_complex \
"[0:v]fade=t=out:st=2.5:d=0.5[v0]; \
 [1:v]fade=t=in:st=0:d=0.5[v1]; \
 [2:v]fade=t=out:st=5.5:d=0.5[v2]; \
 [3:v]fade=t=in:st=0:d=0.5,fade=t=out:st=2.5:d=0.5[v3]; \
 [4:v]fade=t=in:st=0:d=0.5[v4]; \
 [5:v]fade=t=out:st=5.5:d=0.5[v5]; \
 [6:v]fade=t=in:st=0:d=0.5,fade=t=out:st=2.5:d=0.5[v6]; \
 [7:v]fade=t=in:st=0:d=0.5[v7]; \
 [8:v]fade=t=out:st=5.5:d=0.5[v8]; \
 [9:v]fade=t=in:st=0:d=0.5,fade=t=out:st=2.5:d=0.5[v9]; \
 [10:v]fade=t=in:st=0:d=0.5[v10]; \
 [11:v]fade=t=out:st=5.5:d=0.5[v11]; \
 [v0][v1][v2][v3][v4][v5][v6][v7][v8][v9][v10][v11]concat=n=12:v=1:a=0,format=yuv420p[v]" -map "[v]" out.mp4
