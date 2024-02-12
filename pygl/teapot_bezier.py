#!/usr/bin/env python3

import numpy as np
import glfw
import sys
from OpenGL.GL import *
import glm
import math

strVertexShader = """
#version 330

layout(location = 0) in vec4 position;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    gl_Position = P*V*M*vec4(position, 1.0);
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
vp_size_changed=False

#TODO use model
nvertices=16*32
m=glm.mat4()
v=glm.mat4()
p=glm.mat4()


def createShader(shaderType, shaderFile):
    shader = glCreateShader(shaderType)
    glShaderSource(shader, shaderFile) # note that this is a simpler function call than in C

    glCompileShader(shader)

    status = None
    glGetShaderiv(shader, GL_COMPILE_STATUS, status)
    if status == GL_FALSE:
        # Note that getting the error log is much simpler in Python than in C/C++
        # and does not require explicit handling of the string buffer
        strInfoLog = glGetShaderInfoLog(shader)
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
        vertices.append( glm.vec3([float(p) for p in line.split(',')]) )

    return {"patches":patches,"vertices":vertices};

def draw():
    #global vertex_buffer,nvertices,program
    width = glfw.get_framebuffer_size(window)[0]
    height = glfw.get_framebuffer_size(window)[1]
    ratio = width / height;

    glViewport(0, 0, width, height);
    m=glm.identity(glm.mat4)
    m=glm.scale([2.5, 2.5, 2.5]);

    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    p=glm.perspective(math.pi / 2, ratio, 0.1, 50.0)
    glUniformMatrix4fv(m_location, 1, GL_FALSE,  glm.value_ptr(m))
    glUniformMatrix4fv(v_location, 1, GL_FALSE, glm.value_ptr(v))
    glUniformMatrix4fv(p_location, 1, GL_FALSE, glm.value_ptr(p))
    glDrawArrays(GL_TRIANGLES, 0, nvertices)


def resize_cb(window, w, h):
    global vp_size_changed
    vp_size_changed = True

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
    glfw.set_window_size_callback(window, resize_cb)
    return window

def generate_patches(model):
    result=[]
    for p in model['patches']:
        #print(p)
        for i in range(16):
            result.append(model['vertices'][p[i]-1])
        #result.append(v)
    return np.array(result)

m=load_model("../models/teapot")
#print(m['patches'])
#print(m['vertices'])

patches = generate_patches(m)
np.set_printoptions(floatmode="maxprec", precision=4)
for p in patches:
    print(p)

window=init()

shaderList = []

shaderList.append(createShader(GL_VERTEX_SHADER, strVertexShader))
shaderList.append(createShader(GL_FRAGMENT_SHADER, strFragmentShader))

program = glCreateProgram()
glUseProgram(program)

for shader in shaderList:
    glAttachShader(program, shader)
    glLinkProgram(program)


for shader in shaderList:
    glDeleteShader(shader)

vertex_buffer = glGenBuffers(1)

glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)
glBufferData( # PyOpenGL allows for the omission of the size parameter
    GL_ARRAY_BUFFER,
    patches,
    GL_STATIC_DRAW
)

m_location = glGetUniformLocation(program, "M")
v_location = glGetUniformLocation(program, "V")
p_location = glGetUniformLocation(program, "P")

vpos_location=glGetAttribLocation(program, 'position')
glEnableVertexAttribArray(vpos_location);
glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                           0,  None)
glBindBuffer(GL_ARRAY_BUFFER, vpos_location)
glBindVertexArray(glGenVertexArrays(1))


glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)
glEnableVertexAttribArray(0)
glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, None)


#setup camera
# Camera matrix
v =glm.lookAt(glm.vec3(10, 10, 10), # Camera in World Space
            glm.vec3(0, 0, 0),  # and looks at the origin
            glm.vec3(0, 1, 0));

while not glfw.window_should_close(window):
    # Render here, e.g. using pyOpenGL
    draw()
    # Swap front and back buffers
    glfw.swap_buffers(window)

    # Poll for and process events
    glfw.poll_events()
    if vp_size_changed:
        vp_size_changed = False
        w, h = glfw.get_framebuffer_size(window)
        glViewport(0, 0, w, h)
        #print("new viewport size:", w, h)

glfw.terminate()