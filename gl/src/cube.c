
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define nvertices 3*12

vec3 vertices[nvertices] =
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

static const char* vertex_shader_text =
"#version 330 core\n"
"attribute vec3 vPos;\n"
"attribute vec3 vNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 M;\n"
"uniform mat4 V;\n"
"uniform mat4 P;\n"
"void main()\n"
"{\n"
"    FragPos =vec3( M * vec4(vPos, 1.0) );\n"
"    Normal=vNormal;\n"
"    gl_position = P *V * vec4(FragPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;\n"
"void main()\n"
"{\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * vec3(1.0,1.0,1.0);\n"
"    vec3 result = diffuse;\n"
"    FragColor = vec4(result, 1.0);\n"
"    //FragColor = vec3(0.5,1.0,1.0);"
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

void normalMatrix() {
    for(int i=0; i<nvertices-1; i++) {
        vec3_mul_cross(normals[i], vertices[i],vertices[i+1]);
    }
    vec3_mul_cross(normals[nvertices-1],vertices[nvertices-1], vertices[0]);
}

int main(void)
{
    normalMatrix();
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint vpos_location, vcol_location,m_location,v_location,p_location;
    unsigned int cubeVAO;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

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
    GLint normal_location = glGetAttribLocation(program, "vNormal");
    GLint light_location = glGetUniformLocation(program, "lightPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(normal_location);
    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(normals[0]), (void*) 0);

    static vec3 LightPosition  = (vec3){5.0f, 5.0f, 10.0f};

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m,p,mvp,v,mv;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_perspective(p, M_PI/2 , (float) width / (float)height, 0.1f, 100.0f);

        // Camera matrix
        mat4x4_look_at(v,
            (vec3){4,3,3}, // Camera is at (4,3,3), in World Space
            (vec3){0,0,0}, // and looks at the origin
            (vec3){0,1,0}  // Head is up (set to 0,-1,0 to look upside-down)
        );
        mat4x4_mul(mv, v, m);
        mat4x4_mul(mvp, p, mv);

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