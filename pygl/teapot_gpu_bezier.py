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

float detail=16;

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

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout(quads) in;

out vec4 fragpos;
out vec3 fragnormal;

vec3 CP[16];

void bezierDerivative(out float[4] b, out float[4] db, float t) {
	//derivatives

    b[0] = pow(1.0 - t, 3);
	b[1] = 3.0 * pow(1.0 - t, 2) * t;
	b[2] = 3.0 * (1.0 - t) * pow(t,2);
	b[3] = pow(t, 3);

	db[0] = -3.0 * pow(1.0 - t, 2);
	db[1] = -6.0 * (1.0 - t) * t + 3.0 * pow(1.0 - t,2);
	db[2] = -3.0 * pow(t, 2) + 6.0 * t * (1.0 - t);
	db[3] = 3.0 * pow(t, 2);
}

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
    int idx=0;
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	float dbu[4],dbv[4],bu[4],bv[4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            CP[idx]=gl_in[idx].gl_Position.xyz;
            idx++;
        }
    }
    fragpos = evaluateBezier(u,v);
    bezierDerivative(bu,dbu, u);
    bezierDerivative(bv,dbv, v);

    vec3 dPos_du=vec3(0,0,0);
    vec3 dPos_dv=vec3(0,0,0);

    idx=0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dPos_du+=CP[idx]*dbu[i]*bv[j];
            idx++;
        }
    }

    idx=0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dPos_dv+=CP[idx]*bu[i]*dbv[j];
            idx++;
        }
    }

    fragnormal=normalize(cross(dPos_du, dPos_dv));
    gl_Position = P*V*M*fragpos;
}
"""

# String containing fragment shader program written in GLSL
strFragmentShader = """
#version 330 core

in vec4 fragpos;
in vec3 fragnormal;

uniform vec4 lightCamera;
uniform vec3 viewPos;
out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(0.2, 0.2, 0.2);
    vec3 objColor = vec3(1.0,0.2,1.0);

    float specularStrength = 0.5;

    vec3 lightDir = normalize(vec3(lightCamera) - fragpos.xyz);
    float diff = max(dot(fragnormal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragpos.xyz);
    vec3 reflectDir = reflect(-lightDir, fragnormal);

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
camera_location=None

#TODO use model
nvertices=16*32
m=glm.mat4()
v=glm.mat4()
p=glm.mat4()

normal_matrix = glm.mat3()
LightCameraPosition= glm.mat4()

LightPosition = glm.vec4(20.0, 5.0, 40.0, 1.0);
cameraPosition = glm.vec3(10, 10, 10);

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

    for i in range(0,len(control_points),16):
        glDrawArrays( GL_PATCHES, i, 16)

def resize_cb(window, w, h):
    global vp_size_changed
    vp_size_changed = True

def init():
    if not glfw.init():
        print("Unable to get window")
        sys.exit(1)
    # Create a windowed mode window and its OpenGL context

    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 5)
    #glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE)
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

glBufferData(GL_ARRAY_BUFFER, control_points, GL_STATIC_DRAW)

program= load_shaders()

m_location = glGetUniformLocation(program, 'M')
v_location = glGetUniformLocation(program, 'V')
p_location = glGetUniformLocation(program, 'P')
vpos_location = glGetAttribLocation(program, "vpos")
light_location = glGetUniformLocation(program, "lightCamera")
camera_location = glGetUniformLocation(program, "viewPos")
glEnableVertexAttribArray(vpos_location)

glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
            glm.sizeof(glm.vec3), None)
glUseProgram(program)
glEnable(GL_CULL_FACE)
glEnable(GL_DEPTH_TEST)
glPatchParameteri(GL_PATCH_VERTICES, 16)

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