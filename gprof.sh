
cd `dirname $0`

mkdir out

javac src/GraphDrawingVis.java -d out

g++ --std=c++0x -W -Wall -Wno-sign-compare -pipe -mmmx -msse -msse2 -msse3 -pg src/GraphDrawing.cpp
mv a.out out/

java -cp out/ GraphDrawingVis -exec out/a.out -testcase 1

gprof out/a.out gmon.out > log
