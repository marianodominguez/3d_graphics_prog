
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define nvertices 36

static const vec3 vertices[nvertices] =
{
    {-1.0f,-1.0f,-1.0f}, // triangle 1 : begin
    {-1.0f,-1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f}, // triangle 1 : end
    {1.0f, 1.0f,-1.0f}, // triangle 2 : begin
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f}, // triangle 2 : end
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f}
};

static vec3 normals[nvertices];
static vec3 LightPosition  = (vec3){1.2f, 1.0f, 2.0f};

static const char* vertex_shader_text =
"#version 330 core\n"
"attribute vec3 vNormal;\n"
"attribute vec3 vPos;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 M;\n"
"uniform mat4 V;\n"
"uniform mat4 P;\n"
"void main()\n"
"{\n"
"    FragPos =vec3( M * vec4(vPos, 1.0) );\n"
"    Normal = vNormal;\n"
"    gl_Position = P*V*M*vec4(vPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;\n"
"void main()\n"
"{\n"
"    float ambient = 0.2;\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 color = diff+ambient*vec3(1.0,0.2,1.0);\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void makeNormalMatrix() {
    for(int i=0; i<nvertices; i+=3) {
        vec3 ab,ac,bc,ba;
        vec2_sub(ab,normals[i+1], vertices[i]);
        vec2_sub(ac,normals[i+2], vertices[i]);
        vec2_sub(bc,normals[i+2], vertices[i+1]);
        vec2_sub(ba,normals[i], vertices[i+1]);

        vec3_mul_cross(normals[i],ac,ab);
        vec3_mul_cross(normals[i+1],bc,ba);
        vec3_mul_cross(normals[i+2],ac,bc);
    }
}

int main(void)
{
    makeNormalMatrix();
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint p_location,v_location,m_location, vpos_location, vnormal_location,light_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    m_location = glGetUniformLocation(program, "M");
    v_location = glGetUniformLocation(program, "V");
    p_location = glGetUniformLocation(program, "P");
    vpos_location = glGetAttribLocation(program, "vPos");
    vnormal_location = glGetAttribLocation(program, "vNormal");
    light_location = glGetUniformLocation(program, "lightPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vnormal_location);
    glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(normals[0]), (void*) 0 );

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp,v;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        mat4x4_perspective(p, M_PI/2 , (float) width / (float)height, 0.1f, 100.0f);

        // Camera matrix
        mat4x4_look_at(v,
        (vec3){3,2,2}, // Camera in World Space
        (vec3){0,0,0}, // and looks at the origin
        (vec3){0,1,0}  // Head is up (set to 0,-1,0 to look upside-down)
        );
        mat4x4_mul(mvp, v, m);
        mat4x4_mul(mvp, p, mvp);


        glUseProgram(program);
        glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat*) m);
        glUniformMatrix4fv(v_location, 1, GL_FALSE, (const GLfloat*) v);
        glUniformMatrix4fv(p_location, 1, GL_FALSE, (const GLfloat*) p);
        glUniform3fv(light_location,1,(const GLfloat*) LightPosition);
        glDrawArrays(GL_TRIANGLES, 0, 3*12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}