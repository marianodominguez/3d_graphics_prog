# 3dc Graphics implementation

## (Almost) from scratch

Although the only external implementation required is put_pixel, I'm using allegro for creating windows and taking care of double buffering. this can be replaced by any other implamentation.

This is a progression of implementing a basic rendering pipeline, it should work in a PC or raspberry PI

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

-/teapot/teapot_01 ..

Succesive implementation of rendering (WIP)

