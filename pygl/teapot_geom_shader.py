#!/usr/bin/env python3

import numpy as np
import glfw
import sys
from OpenGL.GL import *
import glm
import math

strVertexShader = """
#version 450 core

in vec3 vpos;

void main()
{
    gl_Position = vec4(vpos, 1.0);
}
"""

strGeometryShader= """
#version 450 core

layout(vertices=16) out;

uniform float detail;

void main() {

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = detail;
	gl_TessLevelOuter[1] = detail;
	gl_TessLevelOuter[2] = detail;
	gl_TessLevelOuter[3] = detail;

	gl_TessLevelInner[0] = detail;
	gl_TessLevelInner[1] = detail;

}

"""

strTessellateShader = """
#version 450 core

layout(quads) in;
layout (triangle_strip, max_vertices = 256) out;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
const int numdiv=8;
vec3 CP[16];

vec4 evaluateBezier(float s,float t) {
    vec3 p= vec3(0.0, 0.0, 0.0);
    float b[4], c[4];
    b[0] = (1.0 - t) * (1.0 - t) * (1.0 - t);
    b[1] = 3.0 * t * (1.0 - t) * (1.0 - t);
    b[2] = 3.0 * t * t * (1.0 - t);
    b[3] = t * t * t;

    c[0] = (1.0 - s) * (1.0 - s) * (1.0 - s);
    c[1] = 3.0 * s * (1.0 - s) * (1.0 - s);
    c[2] = 3.0 * s * s * (1.0 - s);
    c[3] = s * s * s;

    int idx =0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            p = p+ b[i] * c[j] * CP[idx];
            idx++;
        }
    }
    return vec4(p,1.0);
}

void main() {
    float dt=1.0/float(numdiv);
    float s=0,t=0;
    vec4 position;
    int idx=0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            CP[idx]=gl_in[idx].gl_Position.xyz;
            idx++;
        }
    }

    for (int i=0; i<numdiv ; i++) {
        s=0.0;
        for (int j=0; j<numdiv;j++ ) {
            position=evaluateBezier(s,t);
            gl_Position = P*V*M*position;
            EmitVertex();
            position=evaluateBezier(s,t+dt);
            gl_Position = P*V*M*position;
            EmitVertex();
            position=evaluateBezier(s+dt,t);
            gl_Position = P*V*M*position;
            EmitVertex();
            EndPrimitive();
            position=evaluateBezier(s,t);
            gl_Position = P*V*M*position;
            EmitVertex();
            position=evaluateBezier(s+dt,t+dt);
            gl_Position = P*V*M*position;
            EmitVertex();
            position=evaluateBezier(s,t+dt);
            gl_Position = P*V*M*position;
            EmitVertex();
            EndPrimitive();
            s+=dt;
        }
        t+=dt;
    }
}
"""

# String containing fragment shader program written in GLSL
strFragmentShader = """
#version 330 core

out vec4 outputColor;
void main()
{
   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
"""

program=None
vertex_buffer=None
vp_size_changed=False
v_location=None
m_location=None
p_location=None
cp_location=None

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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    for i in range(0,len(control_points),16):
        glDrawArrays( GL_TRIANGLE_STRIP_ADJACENCY, i, 16)

def resize_cb(window, w, h):
    global vp_size_changed
    vp_size_changed = True

def init():
    if not glfw.init():
        print("Unable to get window")
        sys.exit(1)
    # Create a windowed mode window and its OpenGL context

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
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
    shaderList.append(createShader(GL_VERTEX_SHADER, strVertexShader))
    shaderList.append(createShader(GL_TESS_CONTROL_SHADER, strGeometryShader))
    shaderList.append(createShader(GL_TESS_EVALUATION_SHADER, strTessellateShader))
    shaderList.append(createShader(GL_FRAGMENT_SHADER, strFragmentShader))
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

glBufferData( # PyOpenGL allows for the omission of the size parameter()
        GL_ARRAY_BUFFER,
        control_points,
        GL_STATIC_DRAW)

program= load_shaders()

m_location = glGetUniformLocation(program, 'M')
v_location = glGetUniformLocation(program, 'V')
p_location = glGetUniformLocation(program, 'P')
vpos_location = glGetAttribLocation(program, "vpos")
glEnableVertexAttribArray(vpos_location)

glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
            glm.sizeof(glm.vec3), None)
glUseProgram(program)

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