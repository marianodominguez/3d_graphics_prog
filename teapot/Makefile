############
MACHINE= $(shell uname -s)

# object files have corresponding source files
OBJS= main.o
CXX=gcc
COMPILER_FLAGS= -g -O -W
#INCLUDE= -I/usr/include/aarch64-linux-gnu
INCLUDE=
LIBS= -lallegro_font -lallegro -lm -lallegro_primitives

EXEC= bin/cube

$(EXEC): $(OBJS)
	$(CXX) $(COMPILER_FLAGS) -o $(EXEC) $(OBJS) $(LIBS)

main.o: src/cube.c
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

polar: src/polar_curves.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

teapot: src/teapot.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

teapot_01: src/teapot_01.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

teapot_02: src/teapot_02.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

teapot_03: src/teapot_03.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

test_bezier: src/test_bezier.c
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ $< $(INCLUDE) $(LIBS)

clean:
	rm -f $(EXEC) $(OBJS)
