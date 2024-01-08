############
MACHINE= $(shell uname -s)

# object files have corresponding source files
OBJS= main.o
CXX=gcc
COMPILER_FLAGS= -g -O -W
#INCLUDE= -I/usr/include/aarch64-linux-gnu
INCLUDE=-I include
LIBS= -lallegro_font -lallegro -lm -lallegro_primitives

cube: clean
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/cube.c $(INCLUDE) $(LIBS)

teapot: clean
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

teapot_01:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_01.c $(INCLUDE) $(LIBS)

teapot_02:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_02.c $(INCLUDE) $(LIBS)

teapot_03:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_03.c $(INCLUDE) $(LIBS)

teapot_04:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_04.c $(INCLUDE) $(LIBS)

clean:
	rm -rf bin/*

all:
	$(CXX) $(COMPILER_FLAGS) -o bin/polar_curves basic/src/polar_curves.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_bezier basic/src/test_bezier.c $(INCLUDE) $(LIBS)
