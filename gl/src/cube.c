
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


vec3 vertices[3*12] =
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
static vec3 normals[3*12];

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"uniform mat4 MV;\n"
"attribute vec3 vPos;\n"
"attribute vec3 vNormal;\n"
"varying vec3 color;\n"
"uniform vec4 LightPosition,DiffuseProduct;\n"

"void main()\n"
"{\n"
"    vec3 position = MV * vec4(vPos, 1.0).xyz;\n"
"    vec3 L = normalize(LightPosition.xyz - position);\n"
"    vec3 V = normalize(-position);\n"
"    vec3 N= normalize( MV*vec4(vNormal,0.0).xyz );\n"
"    vec4 diffuse = max( dot(L,N),0.0)*DiffuseProduct;\n"
"    position = MVP * vec4(vPos, 1.0);\n"
"    color=diffuse;\n"
"    color.a=1.0;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
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
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location,mv_location;

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

    // NOTE: OpenGL error checks have been omitted for brevity

    //glEnable(GL_DEPTH_TEST);
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

    mvp_location = glGetUniformLocation(program, "MVP");
    mv_location = glGetUniformLocation(program, "MV");
    vpos_location = glGetAttribLocation(program, "vPos");
    GLint light_location = glGetUniformLocation(program, "LightPosition");
    GLint diffuse_p_location = glGetUniformLocation(program, "DiffuseProduct");
    GLint normal_location = glGetUniformLocation(program, "vNormal");
    //vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);

    glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(normals[0]), (void*) 0);

    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
      //                    sizeof(vertices[0]), (void*) (sizeof(float) * 2));


    static vec3 LightPosition = (vec3){5.0f, 5.0f, 10.0f};
    static vec3 DiffuseProduct = (vec3){1.0f, 1.0f, 1.0f};

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

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
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

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
        glUniformMatrix4fv(mv_location, 1, GL_FALSE, (const GLfloat*) mv);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform3fv(light_location,1,(const GLfloat*) LightPosition);
        glUniform3fv(diffuse_p_location,1,(const GLfloat*) DiffuseProduct);

        glDrawArrays(GL_TRIANGLES, 0, 3*12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}