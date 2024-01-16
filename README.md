# 3dc Graphics implementation

## (Almost) from scratch

Although the only external library required is put_pixel, I'm using allegro for creating windows and taking care of double buffering. this can be replaced by any other implementation.

This is a progression of writing a basic rendering pipeline, it should work in a PC or raspberry PI

## Installing libraries and requirements

ubuntu/raspbian
most of them are already installed: a c compiler, makefile and basic libraries,
this should cover it.


```
 sudo apt install build-essential
 sudo apt install liballegro5
```

## Runing the program with makefiles

Pass the program name, e.g.

```
cd teapot

make teapot_01

bin/teapot_01
```

## contents

- /basic/src/polar_curves

Demo of graphic library, it shopuld display a spyrograph pattern

- /basic/src/test_bezier

Draws and rotates a bezier patch, interpolating 10 points

- /teapot/cube

Minimal 3D example, a rotating wireframe cube

-/teapot/teapot_01
Wireframe model, projection, translation and rotation

-/teapot/teapot_02
Testing a naive linear interpolation, it does not generate proper polygons.

-/teapot/teapot_03
Wireframe interpolation with Bezier patches

-/teapot/teapot_04
Backface culling, using normals

-/teapot/teapot_05
Triangle filling and rasterization algorithm

-/teapot/teapot_06
Flat shaading


Succesive steps of rendering (WIP)

