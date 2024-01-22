# 3dc Graphics implementation

## (Almost) from scratch

Although the only external library required is put_pixel, I'm using allegro for creating windows and taking care of double buffering. this can be replaced by any other implementation.

This is a progression of writing a basic rendering pipeline, it should work in a PC or raspberry PI

## Installing libraries and requirements

## ubuntu/raspbian

Most of them are already installed: a c compiler, makefile and basic libraries,
this should cover it.


```
 sudo apt install build-essential
 sudo apt install liballegro5
```

## OSX

```
brew install allegro
```

## Runing the program with makefiles

Pass the program name, e.g.

```

make teapot_01

# or 'make all' to create all binaries

bin/teapot_01
```

## Contents

- /basic/src/polar_curves

Demo of graphic library, it shopuld display a spyrograph pattern

- /basic/src/test_bezier

Draws and rotates a bezier patch, interpolating 10 points

- /teapot/cube

Minimal 3D example, a rotating wireframe cube

- /teapot/teapot_01

Wireframe model, projection, translation and rotation

- /teapot/teapot_02

Testing a naive linear interpolation, it does not generate proper polygons.

- /teapot/teapot_03

Wireframe interpolation with Bezier patches

- /teapot/teapot_04

Backface culling, using normals

- /teapot/teapot_05

Triangle filling and rasterization algorithm

- /teapot/teapot_06

Flat shading

- /teapot/teapot_07

Smooth shading, Gouraud algorithm, use derivatives to calculate normals.

- /teapot/teapot_08

Z-Buffer, hiding surfaces.

# References

1. https://www.gabrielgambetta.com/computer-graphics-from-scratch/

1. https://cs.brown.edu/people/jhughes/papers/vanDam-CGP-1995/main.htm

1. https://www.scratchapixel.com/lessons/geometry/bezier-curve-rendering-utah-teapot/bezier-patch-normal.html


