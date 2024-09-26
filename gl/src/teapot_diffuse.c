//#define CGLM_DEFINE_PRINTS 0

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/io.h>

#define nvertices 5144*3
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

GLFWwindow *window;
GLuint vertex_buffer, normal_buffer, vertex_shader, fragment_shader,
    program;
GLint p_location, v_location, m_location, vpos_location, vnormal_location,
    light_location, normal_location, camera_location;
float ratio;
int width, height;

mat4 m, p, mv = GLM_MAT4_IDENTITY_INIT, v;
mat3 normal_matrix = GLM_MAT3_IDENTITY_INIT;
vec4 LightCameraPosition;

static vec3 vertices[nvertices];
static vec3 normals[nvertices];

vec4 LightPosition = (vec4){20.0f, 5.0f, 40.0f, 1.0f};
vec3 cameraPosition = (vec3){10, 10, 10};

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int load_shader(char *filename, GLuint type) {

    FILE *fp;
    long size = 0;
    char* shader_text;

    /* Read File to get size */
    fp = fopen(filename, "rb");
    if(fp == NULL) {
        puts("Unable to load shader");
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(filename, "r");
    shader_text = memset(malloc(size), '\0', size);
    fread(shader_text, 1, size-1, fp);
    fclose(fp);
    GLuint shader = glCreateShader(type);
    GLchar const* shaderC[] = { shader_text };
    glShaderSource(shader, 1, shaderC, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        GLchar errorLog[maxLength];
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        glDeleteShader(shader); // Don't leak the shader.
        puts("Shader compliation failed");
        puts(filename);
        puts(errorLog);
        exit(1);
    }

    return shader;
}

void load_model(char *filename) {
    int size;
    float x,y,z;
    FILE *fp;
    char line[255];

    fp = fopen(filename, "r");

    if(fp == NULL) {
        puts("Unable to load model");
        exit(1);
    }

    fscanf(fp, "%d",&size);

    if (size*3 != nvertices) {
        printf("%s", "Wrong model");
        exit(1);
    }
    int i=0;
    while (i<nvertices) {
        fgets(line,255,fp);
        if (strlen(line) >10 ) {
            sscanf(line, "%f %f %f", &x, &y, &z);
            memcpy (vertices[i], (vec3){x,y,z}, sizeof ((vec3){x,y,z}));
            fgets(line,255,fp);
            sscanf(line, "%f %f %f", &x, &y, &z);
            memcpy (normals[i], (vec3){x,y,z}, sizeof ((vec3){x,y,z}));
            glm_vec3_print(vertices[i],stderr);
            glm_vec3_print(normals[i],stderr);
            i++;
        }
    }
    fclose(fp);
}

void drawScene() {
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_mat4_identity(m);
    glm_scale(m, (vec3){1.5, 1.5, 1.5});
    //glm_rotate_x(m, (float)glfwGetTime()/7.0, m);
    glm_rotate_y(m, (float) glfwGetTime(),m);
    glm_rotate_z(m, (float)glfwGetTime()/5.0, m);

    glm_perspective(M_PI / 4, (float)width / (float)height, 5.0f, 50.0f, p);

    glm_mat4_mul(v, m, mv);

    glm_mat4_pick3(mv, normal_matrix);
    glm_mat3_inv(normal_matrix, normal_matrix);

    glm_mat4_mulv(v, LightPosition, LightCameraPosition);

    glUniformMatrix4fv(m_location, 1, GL_FALSE, (const GLfloat *)m);
    glUniformMatrix4fv(v_location, 1, GL_FALSE, (const GLfloat *)v);
    glUniformMatrix4fv(p_location, 1, GL_FALSE, (const GLfloat *)p);
    // transpose when passing
    glUniformMatrix3fv(normal_location, 1, GL_TRUE,
                        (const GLfloat *)normal_matrix);
    glUniform4fv(light_location, 1, (const GLfloat *)LightCameraPosition);
    glUniform3fv(camera_location, 1, (const GLfloat *)cameraPosition);
    glDrawArrays(GL_TRIANGLES, 0, nvertices);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(void)
{
    load_model("models/teapot_normals.txt");

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays( 1, &vpos_location );
    glBindVertexArray( vpos_location );

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), vertices, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

    program = glCreateProgram();
    vertex_shader   =   load_shader("gl/src/vertex_shader_diff.gsl", GL_VERTEX_SHADER);
    fragment_shader =   load_shader("gl/src/fragment_shader_diff.gsl", GL_FRAGMENT_SHADER);
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
    camera_location = glGetUniformLocation(program, "viewPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), BUFFER_OFFSET(0));
    glVertexAttribPointer(vnormal_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(normals[0]), BUFFER_OFFSET(sizeof(vertices)));
    glEnableVertexAttribArray(vnormal_location);
    glUseProgram(program);

    // Camera matrix
    glm_lookat( cameraPosition,  // Camera in World Space
               (vec3){0, 0, 0},  // and looks at the origin
               (vec3){0, 1, 0}
               // Head is up (set to 0,-1,0 to look upside-down)
               ,v);

    puts(glGetString(GL_VERSION));

    while (!glfwWindowShouldClose(window)) {
        drawScene();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}