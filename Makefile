.MAIN: teapot

############
MACHINE= $(shell uname -s)

# object files have corresponding source files
OBJS= main.o
CXX=gcc
COMPILER_FLAGS= -g -O -W
#INCLUDE= -I/usr/include/aarch64-linux-gnu
INCLUDE=-I include
LIBS= -lallegro_font -lallegro -lm -lallegro_primitives -lallegro_main

cube:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/cube.c $(INCLUDE) $(LIBS)

teapot:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

teapot_01:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_01.c $(INCLUDE) $(LIBS)

teapot_02:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_02.c $(INCLUDE) $(LIBS)

teapot_03:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_03.c $(INCLUDE) $(LIBS)

teapot_04:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_04.c $(INCLUDE) $(LIBS)

teapot_05:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_05.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

all: clean teapot teapot_01 teapot_02 teapot_03 teapot_04 teapot_05 cube
	$(CXX) $(COMPILER_FLAGS) -o bin/polar_curves basic/src/polar_curves.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_bezier basic/src/test_bezier.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_triangle basic/src/test_triangle.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_lines basic/src/test_lines.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

clean:
	rm -rf bin/*