#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: add a function to device.h which allows the retrieval of the
// frame buffer so we can stop relying on the ppu here
#include "device/ppu/ppu.h"
#include "helpers.h"
#include "log.h"

#define WINDOW_SIZE_MULTIPLIER 3
#define WINDOW_WIDTH VIDEO_BUFFER_WIDTH*WINDOW_SIZE_MULTIPLIER
#define WINDOW_HEIGHT VIDEO_BUFFER_HEIGHT*WINDOW_SIZE_MULTIPLIER
#define WINDOW_TITLE "poNES"

#define GL_VERSION_MAJOR 3
#define GL_VERSION_MINOR 2

static GLFWwindow* s_window = NULL;
static GLuint s_vao;
static GLuint s_tex;
static GLuint s_program;

static int s_keymap[kINPUT_SIZE] = {0};

#define ASSIGN_KEYMAP(i, k) s_keymap[i] = k

static InputFlags s_input_state = 0;

#define EXIT_KEY GLFW_KEY_ESCAPE

static const char* s_vert_shader_src =
    "#version 330\n"
    "out vec2 tex_coords;\n"
    "void main() {\n"
    "   vec2 verts[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));\n"
    "   gl_Position = vec4(verts[gl_VertexID], 0, 1);\n"
    "   tex_coords = vec2(0.5*gl_Position.x, -0.5*gl_Position.y)+vec2(0.5);\n"
    "}\n";
static const char* s_frag_shader_src =
    "#version 330\n"
    "uniform sampler2D tex;\n"
    "in vec2 tex_coords;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "   frag_color = texture(tex, tex_coords);\n"
    "}\n";

static void _glfw_error_cb(int error, const char* desc) {
    log_error("glfw error (%d): %s\n", error, desc);
}

static void _input_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;

    // platform specific actions
    switch (key)
    {
        case EXIT_KEY:
            if (action == GLFW_PRESS)
                glfwSetWindowShouldClose(s_window, 1);
            return;
    }
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

    glfwSetKeyCallback(s_window, _input_callback);
    ASSIGN_KEYMAP(kINPUT_UP,        GLFW_KEY_UP);
    ASSIGN_KEYMAP(kINPUT_DOWN,      GLFW_KEY_DOWN);
    ASSIGN_KEYMAP(kINPUT_LEFT,      GLFW_KEY_LEFT);
    ASSIGN_KEYMAP(kINPUT_RIGHT,     GLFW_KEY_RIGHT);
    ASSIGN_KEYMAP(kINPUT_A,         GLFW_KEY_Z);
    ASSIGN_KEYMAP(kINPUT_B,         GLFW_KEY_X);
    ASSIGN_KEYMAP(kINPUT_SELECT,    GLFW_KEY_A);
    ASSIGN_KEYMAP(kINPUT_START,     GLFW_KEY_S);

    glfwMakeContextCurrent(s_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);

    uint32_t buf[VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT];
    for (size_t i = 0; i < VIDEO_BUFFER_WIDTH*VIDEO_BUFFER_HEIGHT; ++i) {
        const size_t x      = i%VIDEO_BUFFER_WIDTH;
        const size_t y      = i/VIDEO_BUFFER_WIDTH;
        const uint32_t r    = (uint32_t)(((float)x / (float)VIDEO_BUFFER_WIDTH) * 255.f);
        const uint32_t g    = (uint32_t)(((float)y / (float)VIDEO_BUFFER_HEIGHT) * 255.f);

        buf[i] = 0xFF000000 | (r << 0) | (g << 8);
    }

    glGenTextures(1, &s_tex);
    glBindTexture(GL_TEXTURE_2D, s_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_BUFFER_WIDTH, VIDEO_BUFFER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);

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
    glDeleteTextures(1, &s_tex);
    glDeleteVertexArrays(1, &s_vao);
    glfwDestroyWindow(s_window);
    glfwTerminate();
}

void platform_poll_events(void) {
    glfwPollEvents();

    for (size_t i = 0; i < kINPUT_SIZE; ++i) {
        const int key   = s_keymap[i];
        const int state = glfwGetKey(s_window, key);

        write_bit(&s_input_state, i, state == GLFW_PRESS);
    }
}

InputFlags platform_get_inputs(void) {
    return s_input_state;
}

int platform_is_running(void) {
    return ! glfwWindowShouldClose(s_window);
}

void platform_update_frame_buffer(uint32_t* buffer) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, VIDEO_BUFFER_WIDTH, VIDEO_BUFFER_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

void platform_draw(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(s_window);
}

