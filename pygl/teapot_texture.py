#!/usr/bin/env python3

import numpy as np
import glfw
import sys
from OpenGL.GL import *
import glm
import math
from PIL import Image

program=None
vertex_buffer=None
vp_size_changed=False
v_location=None
m_location=None
p_location=None
cp_location=None
camera_location=None
texture_location=None

#TODO use model
nvertices=16*32
m=glm.mat4()
v=glm.mat4()
p=glm.mat4()

normal_matrix = glm.mat3()
LightCameraPosition= glm.mat4()

LightPosition = glm.vec4(20.0, 5.0, 40.0, 1.0);
cameraPosition = glm.vec3(10, 10, 10);

def read_texture(filename):
    img = Image.open(filename)
    img_data = np.array(list(img.getdata()), np.int8)
    textID = glGenTextures(1)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.size[0], img.size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, img_data)
    return textID

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
        elif shaderType is GL_TESS_CONTROL_SHADER:
            strShaderType = "Tesselation Control shader"
        elif shaderType is GL_TESS_EVALUATION_SHADER:
            strShaderType = "Tesselation Evaluation shader"
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
    global v,p,m
    width = glfw.get_framebuffer_size(window)[0]
    height = glfw.get_framebuffer_size(window)[1]
    ratio = width / height;

    glViewport(0, 0, width, height);

    m=glm.mat4(1.0)
    m=glm.scale(m, glm.vec3(0.9, 0.9, 0.9))
    m=glm.rotate(m, glfw.get_time()/7.0, glm.vec3(1,0,0))
    m=glm.rotate(m, glfw.get_time(),   glm.vec3(0,1,0) )

    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    p=glm.perspective(math.pi / 8, ratio, 5, 30.0)
    glUniformMatrix4fv(m_location, 1, GL_FALSE, glm.value_ptr(m))
    glUniformMatrix4fv(v_location, 1, GL_FALSE, glm.value_ptr(v))
    glUniformMatrix4fv(p_location, 1, GL_FALSE, glm.value_ptr(p))

    LightCameraPosition=v*LightPosition

    glUniform4fv(light_location, 1, glm.value_ptr(LightCameraPosition))
    glUniform3fv(camera_location, 1, glm.value_ptr(cameraPosition))
    #glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glDrawArrays( GL_PATCHES, 0, len(control_points))

def resize_cb(window, w, h):
    global vp_size_changed
    vp_size_changed = True

def init():
    if not glfw.init():
        print("Unable to get window")
        sys.exit(1)
    # Create a windowed mode window and its OpenGL context

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 1)
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
    glfw.window_hint(glfw.OPENGL_DEBUG_CONTEXT, GL_TRUE)

    window = glfw.create_window(640, 480, "Utah teapot", None, None)
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
        for i in range(len(p)):
            result.append(model['vertices'][p[i]-1])
    return result

def load_shaders():
    shaderList = []
    shaderList.append(createShader(GL_VERTEX_SHADER, open("shaders/vertex_texture.gsl")))
    shaderList.append(createShader(GL_TESS_EVALUATION_SHADER, open("shaders/tess_eval.gsl")))
    shaderList.append(createShader(GL_TESS_CONTROL_SHADER, open("shaders/tess_ctl_texture.gsl")))
    shaderList.append(createShader(GL_FRAGMENT_SHADER, open("shaders/frag_texture.gsl")))
    program = glCreateProgram()

    for shader in shaderList:
        glAttachShader(program, shader)
        glLinkProgram(program)
        if not glGetProgramiv( program, GL_LINK_STATUS ):
            print( 'link error:' )
            print( glGetProgramInfoLog( program ) )
        glDeleteShader(shader)
    return program

np.set_printoptions(floatmode="maxprec", precision=4)

model=load_model("../models/teapot")
patches=model['patches']
#print(m['vertices'])

#dummy_vertices=[]
control_points = np.array(generate_patches(model))
print(control_points)

window=init()
vertex_attributes=glGenVertexArrays(1)
glBindVertexArray(vertex_attributes)

vertex_buffer = glGenBuffers(1)
glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)

glBufferData(GL_ARRAY_BUFFER, control_points, GL_STATIC_DRAW)

program= load_shaders()

m_location = glGetUniformLocation(program, 'M')
v_location = glGetUniformLocation(program, 'V')
p_location = glGetUniformLocation(program, 'P')
vpos_location = glGetAttribLocation(program, "vpos")
light_location = glGetUniformLocation(program, "lightCamera")
camera_location = glGetUniformLocation(program, "viewPos")
texture_location = glGetAttribLocation(program, "aTextCoord")

glEnableVertexAttribArray(vpos_location)
glEnableVertexAttribArray(texture_location);

glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
            glm.sizeof(glm.vec3), None)
offset = glm.sizeof(glm.vec3)*len(control_points)
glVertexAttribPointer(texture_location, 2, GL_FLOAT, GL_FALSE,
                          glm.sizeof(glm.vec3), ctypes.c_void_p(offset))
glUseProgram(program)
glEnable(GL_CULL_FACE)
glEnable(GL_DEPTH_TEST)
glPatchParameteri(GL_PATCH_VERTICES, 16)
texture_id = read_texture('../textures/marble.jpg')
glBindTexture(GL_TEXTURE_2D, texture_id)

#setup camera
# Camera matrix
v =glm.lookAt(glm.vec3(10, 10, 10), # Camera in World Space
            glm.vec3(0, 0, 0),  # and looks at the origin
            glm.vec3(0, 1, 0))

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
glfw.terminate()
