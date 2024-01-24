
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


static const struct
{
    float x, y,z;
} vertices[3*12] =
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

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"

"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vec3(1.0,1.0,1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
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

int main(void)
{
    float ratio;
    int width, height;
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()){
        printf("Unable to init opengl");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        printf("Unable to create window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glEnable(GL_DEPTH_TEST);
    glGenBuffers(1, &vertex_buffer);
    //glBindVertexArray (vertices[0]);
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

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);

    printf("vendor : %s\n", glGetString(GL_VENDOR) );
    printf("renderer: %s\n", glGetString(GL_RENDERER) );
    printf("version: %s\n", glGetString(GL_VERSION) );
    printf("shading ver: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClearDepth(1.0);
    glViewport(0, 0, width, height);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    while (!glfwWindowShouldClose(window))
    {
        mat4x4 m, p, mvp,v;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        mat4x4_perspective(p, M_PI/2 , (float) width / (float)height, 0.1f, 100.0f);

        // Camera matrix
        mat4x4_look_at(v,
        (vec3){4,3,3}, // Camera is at (4,3,3), in World Space
        (vec3){0,0,0}, // and looks at the origin
        (vec3){0,1,0}  // Head is up (set to 0,-1,0 to look upside-down)
        );
        mat4x4_mul(mvp, v, m);
        mat4x4_mul(mvp, p, mvp);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3*12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}