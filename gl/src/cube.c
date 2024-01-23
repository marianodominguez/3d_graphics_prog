
#define GLEW_STATIC
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

GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

static const char* vertex_shader_text =
"#version 150\n"
"uniform mat4 camera;\n"
"uniform mat4 model;\n"
"in vec3 vert;\n"
"in vec2 vertTexCoord;\n"
"in vec3 vertNormal;\n"
"out vec3 fragVert;\n"
"out vec2 fragTexCoord;\n"
"out vec3 fragNormal;\n"
"void main() {\n"
"    // Pass some variables to the fragment shader\n"
"    fragTexCoord = vertTexCoord;\n"
"    fragNormal = vertNormal;\n"
"    fragVert = vert;\n"
"\n"
"    // Apply all matrix transformations to vert\n"
"    gl_Position = camera * model * vec4(vert, 1);\n"
"}\n";

static const char* fragment_shader_text =
"#version 150\n"
"\n"
"uniform mat4 model;\n"
"uniform sampler2D tex;\n"
"\n"
"uniform struct Light {\n"
"   vec3 position;\n"
"   vec3 intensities; //a.k.a the color of the light\n"
"} light;\n"
"in vec2 fragTexCoord;\n"
"in vec3 fragNormal;\n"
"in vec3 fragVert;\n"
"out vec4 finalColor;\n"
"void main() {\n"
"    //calculate normal in world coordinates\n"
"    mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
"    vec3 normal = normalize(normalMatrix * fragNormal);\n"
"\n"
"    //calculate the location of this fragment (pixel) in world coordinates\n"
"    vec3 fragPosition = vec3(model * vec4(fragVert, 1));\n"
"\n"
"    //calculate the vector from this pixels surface to the light source\n"
"    vec3 surfaceToLight = light.position - fragPosition;\n"
"\n"
"    //calculate the cosine of the angle of incidence\n"
"    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));\n"
"    brightness = clamp(brightness, 0, 1);\n"
"\n"
"    //calculate final color of the pixel, based on:\n"
"    // 1. The angle of incidence: brightness\n"
"    // 2. The color/intensities of the light: light.intensities\n"
"    // 3. The texture and texture coord: texture(tex, fragTexCoord)\n"
"    vec4 surfaceColor = texture(tex, fragTexCoord);\n"
"    finalColor = vec4(brightness * light.intensities * surfaceColor.rgb, surfaceColor.a);\n"
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
    GLint mvp_location, vpos_location, vcol_location;

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
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

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
    vpos_location = glGetAttribLocation(program, "vPos");
    //vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
      //                    sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);

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