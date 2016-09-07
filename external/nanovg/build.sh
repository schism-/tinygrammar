clang++ -std=c++14 -stdlib=libc++ -O2 -c -o nanovg.o nanovg.cpp
clang++ -std=c++14 -stdlib=libc++ -O2 -c -o nanovg_gl2.o nanovg_gl2.cpp
ar rcs libnanovg.a nanovg.o
rm *.o
