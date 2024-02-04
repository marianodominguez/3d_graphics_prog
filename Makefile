.PHONY: default all teapot
default: teapot ;

############
MACHINE= $(shell uname -s)
GL_LIBS= -lglfw -lGLEW -lm -framework OpenGL
ifeq ($(MACHINE),Linux)
	GL_LIBS=-lglfw -lGLEW -lm -lGL
endif

# object files have corresponding source files
OBJS= main.o
CXX=gcc
COMPILER_FLAGS= -g -O0 -Wno-parentheses-equality
#INCLUDE= -I/usr/include/aarch64-linux-gnu
INCLUDE=-I include -I /usr/local/include
LIBS= -lallegro_font -lallegro -lm -lallegro_primitives -lallegro_main -lcglm

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

teapot_06:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_06.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

teapot_07:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_07.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

teapot_08:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ teapot/src/teapot_08.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

triangle_color:
	$(CXX) $(COMPILER_FLAGS) -o bin/test_triangle_color basic/src/test_triangle_color.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)

all: clean teapot_01 teapot_02 teapot_03 teapot_04 teapot_05 teapot_06 teapot_07 teapot_08 cube teapot triangle_color
	$(CXX) $(COMPILER_FLAGS) -o bin/polar_curves basic/src/polar_curves.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_bezier basic/src/test_bezier.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_triangle basic/src/test_triangle.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)
	$(CXX) $(COMPILER_FLAGS) -o bin/test_lines basic/src/test_lines.c teapot/src/auxiliar.c $(INCLUDE) $(LIBS)


gl_teapot:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ gl/src/teapot.c $(INCLUDE) $(GL_LIBS)

gl_teapot_diffuse:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ gl/src/teapot_diffuse.c $(INCLUDE) $(GL_LIBS)

gl_teapot_specular:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ gl/src/teapot_spec.c $(INCLUDE) $(GL_LIBS)


gl_cube:
	$(CXX) $(COMPILER_FLAGS) -o bin/$@ gl/src/cube.c $(INCLUDE) $(GL_LIBS

clean:
	rm -rf bin/*