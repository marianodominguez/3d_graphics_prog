############
MACHINE= $(shell uname -s)

# object files have corresponding source files
OBJS= main.o
CXX=gcc
COMPILER_FLAGS= -g -O -W
#INCLUDE= -I/usr/include/aarch64-linux-gnu
INCLUDE=
LIBS= -lallegro_font -lallegro -lm -lallegro_primitives

EXEC= bin/*

all:
	$(CXX) $(COMPILER_FLAGS) -o bin/polar_curves src/polar_curves.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_bezier src/test_bezier.c $(INCLUDE) $(LIBS)

clean:
	rm -f $(EXEC) $(OBJS)
