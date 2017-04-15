
cd `dirname $0`

mkdir out

javac src/GraphDrawingVis.java -d out

g++ --std=c++0x -W -Wall -Wno-sign-compare -O2 -s -pipe -mmmx -msse -msse2 -msse3 src/GraphDrawing.cpp
mv a.out out/

java -cp out/ GraphDrawingVis -exec out/a.out
