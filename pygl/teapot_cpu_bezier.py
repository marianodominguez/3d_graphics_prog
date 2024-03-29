#!/usr/bin/env python3

import numpy as np
import glfw
import sys
from OpenGL.GL import *
import glm
import math

strVertexShader = """
#version 330 core

in vec3 vPos;
in vec3 vNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform mat3 normal_matrix;

void main()
{
    FragPos =vec3( M * vec4(vPos, 1.0) );
    Normal = normal_matrix * vNormal;
    gl_Position = P*V*M*vec4(vPos, 1.0);
}
"""

# String containing fragment shader program written in GLSL
strFragmentShader = """
#version 330 core
in vec3 Normal;
in vec3 FragPos;

uniform vec4 lightCamera;
uniform vec3 viewPos;
out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(0.2, 0.2, 0.2);
    vec3 objColor = vec3(1.0,0.2,1.0);

    float specularStrength = 0.5;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(lightCamera) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0,1.0,1.0);

    vec3 color = (diff+specular+ambient)*objColor;

    FragColor = vec4(color, 1.0);
}
"""

program=None
vertex_buffer=None
vp_size_changed=False
v_location=None
m_location=None
p_location=None
cp_location=None
normal_location=None
camera_location=None

normal_matrix = glm.mat3()
LightCameraPosition= glm.mat4()

LightPosition = glm.vec4(20.0, 5.0, 40.0, 1.0);
cameraPosition = glm.vec3(10, 10, 10);

NUMBER_OF_DIVISIONS=8

#TODO use model
nvertices=16*32
m=glm.mat4()
v=glm.mat4()
p=glm.mat4()
mv=glm.mat4()
vertices = []
normals = []

def derivativeBezier(u ,x0,x1,x2,x3) :
    return -3 * (1 - u) * (1 - u) * x0 + (3 * (1 - u) *(1 - u) - 6 * u * (1 - u)) \
            * x1 + (6 * u * (1 - u) - 3 * u * u) \
            * x2 + 3 * u * u * x3

def bezier_2d(C,t):
    p = glm.vec3(0, 0, 0)
    b = [ 0 for i in range(4) ]
    b[0] = (1 - t) * (1 - t) * (1 - t)
    b[1] = 3 * t * (1 - t) * (1 - t)
    b[2] = 3 * t * t * (1 - t)
    b[3] = t * t * t

    for i in range(4):
        p= p+ b[i] * C[i];
    return p

def dUBezier(C,u,v) :
    P=[ 0.0 for i in range(4) ]
    vCurve=[ 0 for i in range(4) ]
    r=glm.vec3()
    for i in range(4):
       P[0] = C[0][i]
       P[1] = C[1][i]
       P[2] = C[2][i]
       P[3] = C[3][i]
       vCurve[i] = bezier_2d(P, v)

    r=derivativeBezier(u, vCurve[0],vCurve[1],vCurve[2],vCurve[3])
    return r

def dVBezier(C,u,v):
    uCurve=[ 0 for i in range(4)]
    r=glm.vec3()
    for i in range(4):
       uCurve[i] = bezier_2d(C[i], u)

    r=derivativeBezier(v, uCurve[0],uCurve[1],uCurve[2],uCurve[3])
    return r

def bezier_normal(u,v,C):
    N=glm.vec3()
    dU = dUBezier(C, u, v);
    dV = dVBezier(C, u, v);
    N = glm.normalize(glm.cross(dU,dV));
    return N;

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
    global v,m,mv
    width = glfw.get_framebuffer_size(window)[0]
    height = glfw.get_framebuffer_size(window)[1]
    ratio = width / height;

    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    glViewport(0, 0, width, height);
    m=glm.mat4(1.0)
    m=glm.scale(m, glm.vec3(0.9, 0.9, 1.0))
    m=glm.rotate(m, glfw.get_time()/7.0, glm.vec3(1,0,0))
    m=glm.rotate(m, glfw.get_time(),   glm.vec3(0,1,0) )

    p=glm.perspective(math.pi / 8, ratio, 5, 30.0)
    glUniformMatrix4fv(m_location, 1, GL_FALSE, glm.value_ptr(m))
    glUniformMatrix4fv(v_location, 1, GL_FALSE, glm.value_ptr(v))
    glUniformMatrix4fv(p_location, 1, GL_FALSE, glm.value_ptr(p))

    mv=v*m

    normal_matrix=glm.mat3(mv)
    normal_matrix=glm.inverseTranspose(normal_matrix)

    LightCameraPosition=v*LightPosition

    glUniformMatrix3fv(normal_location, 1, GL_FALSE,
                        glm.value_ptr(normal_matrix))
    glUniform4fv(light_location, 1, glm.value_ptr(LightCameraPosition))
    glUniform3fv(camera_location, 1, glm.value_ptr(cameraPosition))

    #glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    step=(NUMBER_OF_DIVISIONS*NUMBER_OF_DIVISIONS)*6

    for i in range(0,len(vertices), step):
        glDrawArrays( GL_TRIANGLES, i,  step)
    ##glDrawArrays( GL_TRIANGLES, , 16 )

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
        v=[]
        for i in range(16):
            v.append( model['vertices'][p[i]-1] )
        m=[[ 0 for i in range(4)] for j in range(4) ]
        idx=0
        for i in range(4):
            for j in range(4):
                m[i][j] = v[idx]
                idx+=1
        result.append(m)
    return result

def load_shaders():
    shaderList = []
    shaderList.append(createShader(GL_VERTEX_SHADER, strVertexShader))
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

def evaluate_bezier(s,t,CP):
    p = glm.vec3(0, 0, 0)
    b=[0.0 for i in range(4)]
    c=[0.0 for i in range(4)]

    b[0] = (1 - t) * (1 - t) * (1 - t)
    b[1] = 3 * t * (1 - t) * (1 - t)
    b[2] = 3 * t * t * (1 - t)
    b[3] = t * t * t

    c[0] = (1 - s) * (1 - s) * (1 - s)
    c[1] = 3 * s * (1 - s) * (1 - s)
    c[2] = 3 * s * s * (1 - s)
    c[3] = s * s * s

    for i in range(4):
        for j in range(4):
            p = p + b[i] * c[j] * CP[i][j]
    return p

def generate_bezier(CP):
    result = {"control_points": [], "normals":[]}
    t=0.0
    ndiv=NUMBER_OF_DIVISIONS
    dt = 1.0/ndiv
    for i in range(ndiv):
        s=0.0
        for j in range(ndiv):
            p = evaluate_bezier(s,t,CP)
            n = bezier_normal(s,t,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            p = evaluate_bezier(s+dt,t,CP)
            n = bezier_normal(s+dt,t,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            p = evaluate_bezier(s+dt,t+dt,CP)
            n = bezier_normal(s+dt,t+dt,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            p = evaluate_bezier(s,t,CP)
            n = bezier_normal(s,t,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            p = evaluate_bezier(s+dt,t+dt,CP)
            n = bezier_normal(s+dt,t+dt,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            p = evaluate_bezier(s,t+dt,CP)
            n = bezier_normal(s,t+dt,CP)
            result["control_points"].append(p)
            result["normals"].append(n)
            s+=dt
        t+=dt
    return result

np.set_printoptions(floatmode="maxprec", precision=4)

model=load_model("../models/teapot")
patchlist = generate_patches(model)

for control_points in patchlist:
    #print(control_points)
    #print("-----------------")
    p = generate_bezier(control_points)
    vertices.extend(p['control_points'])
    normals.extend(p['normals'])

window=init()
vertex_attributes=glGenVertexArrays(1)
glBindVertexArray(vertex_attributes)

vertex_buffer = glGenBuffers(1)
glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer)

glBufferData( # PyOpenGL allows for the omission of the size parameter()
        GL_ARRAY_BUFFER,
        glm.sizeof(glm.vec3)*(len(vertices)+len(normals)),
        np.array(vertices),
        GL_STATIC_DRAW)

offset = glm.sizeof(glm.vec3)*len(vertices)
glBufferSubData(GL_ARRAY_BUFFER, offset, np.array(normals) )

program= load_shaders()

m_location = glGetUniformLocation(program, 'M')
v_location = glGetUniformLocation(program, 'V')
p_location = glGetUniformLocation(program, 'P')
vpos_location = glGetAttribLocation(program, "vPos")
vnormal_location = glGetAttribLocation(program, "vNormal")
normal_location = glGetUniformLocation(program, "normal_matrix")
light_location = glGetUniformLocation(program, "lightCamera")
camera_location = glGetUniformLocation(program, "viewPos")

glEnableVertexAttribArray(vpos_location)

glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
            glm.sizeof(glm.vec3), None)
glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE,
            glm.sizeof(glm.vec3), ctypes.c_void_p(offset))
glEnableVertexAttribArray(vnormal_location);
glUseProgram(program)

glEnable(GL_CULL_FACE)
glEnable(GL_DEPTH_TEST)

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