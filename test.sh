
cd `dirname $0`

mkdir out

javac src/GraphDrawingVis.java -d out

g++ src/GraphDrawing.cpp
mv a.out out/

java -cp out/ GraphDrawingVis -exec out/a.out
