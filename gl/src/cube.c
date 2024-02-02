
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define nvertices 36

static const vec3 vertices[nvertices] = {
    {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},

    {-0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},

    {-0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},

    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},

    {-0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f},
    {-0.5f, -0.5f, 0.5f},
    {-0.5f, -0.5f, -0.5f},

    {-0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, 0.5f, -0.5f}};

static vec3 normals[nvertices] = {
    {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},

    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},

    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},

    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},

    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f},

    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}};

vec4 LightPosition = (vec4){2.0f, 12.0f, 5.0f, 1.0f};

static const char *vertex_shader_text =
    "#version 330 core\n"
    "attribute vec3 vNormal;\n"
    "attribute vec3 vPos;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "uniform mat4 M;\n"
    "uniform mat4 V;\n"
    "uniform mat4 P;\n"
    "uniform mat3 normal_matrix;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragPos =vec3( M * vec4(vPos, 1.0) );\n"
    "    Normal = normal_matrix * vNormal;\n"
    "    gl_Position = P*V*M*vec4(vPos, 1.0);\n"
    "}\n";

static const char *fragment_shader_text =
    "#version 330 core\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "uniform vec4 lightCamera;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    float ambient = 0.2;\n"
    "    vec3 norm = normalize(Normal);\n"
    "    vec3 lightDir = normalize(vec3(lightCamera) - FragPos);\n"
    "    float diff = max(dot(norm, lightDir), 0.0);\n"
    "    vec3 color = diff+ambient*vec3(1.0,0.2,1.0);\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow *window;
    GLuint vertex_buffer, normal_buffer, vertex_shader, fragment_shader,
        program;
    GLint p_location, v_location, m_location, vpos_location, vnormal_location,
        light_location, normal_location;
    float ratio;
    int width, height;

    mat4 m, p, mv = GLM_MAT4_IDENTITY_INIT, v;
    mat3 normal_matrix = GLM_MAT3_IDENTITY_INIT;
    vec4 LightCameraPosition;

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
    normal_location = glGetUniformLocation(program, "normal_matrix");
    light_location = glGetUniformLocation(program, "lightCamera");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);
    glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(normals[0]), (void *)0);
    glEnableVertexAttribArray(vnormal_location);
    glUseProgram(program);

    // Camera matrix
    glm_lookat((vec3){2, 2, 3}, // Camera in World Space
               (vec3){0, 0, 0}, // and looks at the origin
               (vec3){0, 1, 0}
               // Head is up (set to 0,-1,0 to look upside-down)
               ,
               v);

    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm_mat4_identity(m);
        glm_scale(m, (vec3){2.5, 2.5, 2.5});
        glm_rotate_x(m, (float)glfwGetTime(), m);
        // glm_rotate_y(m, (float) glfwGetTime(),m);
        glm_rotate_z(m, (float)glfwGetTime(), m);

        glm_perspective(M_PI / 2, (float)width / (float)height, 0.1f, 50.0f, p);

        glm_mat4_mul(v, m, mv);

        glm_mat4_pick3(mv, normal_matrix);
        glm_mat3_inv(normal_matrix, normal_matrix);
        glm_mat3_transpose(normal_matrix);

        glm_mat4_mulv(v, LightPosition, LightCameraPosition);

        glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat *)m);
        glUniformMatrix4fv(v_location, 1, GL_FALSE, (const GLfloat *)v);
        glUniformMatrix4fv(p_location, 1, GL_FALSE, (const GLfloat *)p);

        glUniformMatrix3fv(normal_location, 1, GL_FALSE,
                           (const GLfloat *)normal_matrix);
        glUniform4fv(light_location, 1, (const GLfloat *)LightCameraPosition);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
