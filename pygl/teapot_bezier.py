#!/usr/bin/env python3

import numpy as np
import glfw
import sys
from OpenGL.GL import *

strVertexShader = """
#version 330

layout(location = 0) in vec4 position;
void main()
{
   gl_Position = position;
}
"""

# String containing fragment shader program written in GLSL
strFragmentShader = """
#version 330

out vec4 outputColor;
void main()
{
   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
"""

program=None
vertex_buffer=None

#TODO use model
nvertices=3

vertices = np.array(
    [0.75, 0.75, 0.0, 1.0,
    0.75, -0.75, 0.0, 1.0,
    -0.75, -0.75, 0.0, 1.0],
    dtype='float32'
)

def createShader(shaderType, shaderFile):
    shader = glCreateShader(shaderType)
    glShaderSource(shader, shaderFile) # note that this is a simpler function call than in C

    glCompileShader(shader)

    status = None
    glGetShaderiv(shader, GL_COMPILE_STATUS, status)
    if status == GL_FALSE:
        # Note that getting the error log is much simpler in Python than in C/C++
        # and does not require explicit handling of the string buffer
        strInfoLog = glGetShaderInforLog(shader)
        strShaderType = ""
        if shaderType is GL_VERTEX_SHADER:
            strShaderType = "vertex"
        elif shaderType is GL_GEOMETRY_SHADER:
            strShaderType = "geometry"
        elif shaderType is GL_FRAGMENT_SHADER:
            strShaderType = "fragment"

        print("Compilation failure for " + strShaderType + " shader:\n" + strInfoLog)

    return shader

def load_model(filename):
    patches=[]
    vertices=[]
    f=open(filename)
    npatches=int(f.readline())
    if npatches!=32:
        raise Exception("Incorrect model format")
    for i in range(npatches):
        line=f.readline()
        patches.append([int(x) for x in line.split(',')])
    nvertices=int(f.readline())
    for i in range(nvertices):
        line=f.readline()
        vertices.append( np.array([float(p) for p in line.split(',')]) )

    return {"patches":patches,"vertices":vertices};

def draw():
    #global vertex_buffer,nvertices,program
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClear(GL_COLOR_BUFFER_BIT)
    glUseProgram(program)

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)
    glEnableVertexAttribArray(0)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, None)

    glDrawArrays(GL_TRIANGLES, 0, nvertices)

    glDisableVertexAttribArray(0)
    glUseProgram(0)


def init():
    if not glfw.init():
        print("Unable to get window")
        sys.exit(1)
    # Create a windowed mode window and its OpenGL context
    window = glfw.create_window(640, 480, "Hello World", None, None)
    if not window:
        glfw.terminate()
        sys.exit(1)

    # Make the window's context current
    glfw.make_context_current(window)
    return window

#m=load_model("../models/teapot")
#print(m['patches'])
#print(m['vertices'])
window=init()

shaderList = []

shaderList.append(createShader(GL_VERTEX_SHADER, strVertexShader))
shaderList.append(createShader(GL_FRAGMENT_SHADER, strFragmentShader))

program = glCreateProgram()

for shader in shaderList:
    glAttachShader(program, shader)
    glLinkProgram(program)

for shader in shaderList:
    glDeleteShader(shader)

vertex_buffer = glGenBuffers(1)

glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)
glBufferData( # PyOpenGL allows for the omission of the size parameter
    GL_ARRAY_BUFFER,
    vertices,
    GL_STATIC_DRAW
)
glBindBuffer(GL_ARRAY_BUFFER, 0)
glBindVertexArray(glGenVertexArrays(1))


while not glfw.window_should_close(window):
    # Render here, e.g. using pyOpenGL
    draw()
    # Swap front and back buffers
    glfw.swap_buffers(window)

    # Poll for and process events
    glfw.poll_events()

glfw.terminate()