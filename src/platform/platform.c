#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "device/ppu.h"
#include "log.h"

#define WINDOW_SIZE_MULTIPLIER 2
#define WINDOW_WIDTH VIDEO_BUFFER_WIDTH*WINDOW_SIZE_MULTIPLIER
#define WINDOW_HEIGHT VIDEO_BUFFER_HEIGHT*WINDOW_SIZE_MULTIPLIER
#define WINDOW_TITLE "poNES"

#define GL_VERSION_MAJOR 3
#define GL_VERSION_MINOR 2

static GLFWwindow* s_window = NULL;
static GLuint s_vao;
static GLuint s_program;

static const char* s_vert_shader_src =
    "#version 330\n"
    "out vec2 tex_coords;\n"
    "void main() {\n"
    "   vec2 verts[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));\n"
    "   gl_Position = vec4(verts[gl_VertexID], 0, 1);\n"
    "   tex_coords = 0.5*gl_Position.xy+vec2(0.5);\n"
    "}\n";
static const char* s_frag_shader_src =
    "#version 330\n"
    "in vec2 tex_coords;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = vec4(tex_coords.xy, 0, 1);\n"
    "}\n";

static void _glfw_error_cb(int error, const char* desc) {
    log_error("glfw error (%d): %s\n", error, desc);
}

int platform_init(void) {
    if (! glfwInit()) {
        log_error("failed to init glfw");
        return 0;
    }

    glfwSetErrorCallback(_glfw_error_cb);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    s_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (! s_window) {
        log_error("failed to create window");
        return 0;
    }

    glfwMakeContextCurrent(s_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);

    const GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &s_vert_shader_src, NULL);
    glCompileShader(vert_shader);

    const GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &s_frag_shader_src, NULL);
    glCompileShader(frag_shader);

    s_program = glCreateProgram();
    glAttachShader(s_program, vert_shader);
    glAttachShader(s_program, frag_shader);
    glLinkProgram(s_program);
    glUseProgram(s_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return 1;
}

void platform_cleanup(void) {
    glDeleteProgram(s_program);
    glDeleteVertexArrays(1, &s_vao);
    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void platform_poll_events(void) {
    glfwPollEvents();
}

int platform_is_running(void) {
    return ! glfwWindowShouldClose(s_window);
}

void platform_draw(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(s_window);
}

