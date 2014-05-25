#!/bin/sh
COMMAND4="g++ -std=c++0x  -O3 `pkg-config opencv --cflags` -g  distance.cpp -o distance `pkg-config opencv --libs`";

COMMAND="g++ -std=c++0x `pkg-config opencv --cflags` distance.cpp `pkg-config opencv --cflags` -I/usr/include/boost -L/usr/share/ -lboost_system -lpthread `pkg-config opencv --libs` -o distance";

COMPILESTARTUP="gcc startup.c -o startup"

COMMANDBEER="g++ -std=c++0x -g `pkg-config opencv --cflags` distance.cpp `pkg-config opencv --cflags` -I/usr/include/boost -L/usr/share/ -lboost_system -lpthread `pkg-config opencv --libs` -o distance";

COMMAND5="g++ distance_linux.cpp `pkg-config opencv --cflags` -I/usr/include/boost -L/usr/share/ -lboost_system -lpthread `pkg-config opencv --libs` -o distance_linux";

#echo $COMMAND4
#$COMMAND4
echo $COMMANDBEER
$COMMANDBEER

echo $COMPILESTARTUP
$COMPILESTARTUP

echo "done compiling"
