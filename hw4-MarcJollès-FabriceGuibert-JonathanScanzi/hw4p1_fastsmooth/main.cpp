// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "glm/gtc/matrix_transform.hpp"

#include "framebuffer.h"

#include "cube/cube.h"
#include "quad/quad.h"
#include "screenquad/screenquad.h"


Cube cube;
Quad quad;

int window_width = 800;
int window_height = 600;

vector<float> G;

FrameBuffer framebuffer;
ScreenQuad screenquad;
ScreenQuad screenquad2;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 cube_model_matrix;

size_t MAX_SIZE = 400; // /!\ modify too in screenquad_fshader if modified
float gaussian_std = 2.0;

void regenerateG() {

    int SIZE = 1 + 2 * 3 * int(ceil(gaussian_std));

    G.clear();
    //We choose these boundaries, as they are the same as for basic gaussian blur
    for(int i = -SIZE; i <= SIZE; ++i) {
        G.push_back(exp(-(i*i)/(2.0*gaussian_std*gaussian_std*gaussian_std*gaussian_std)));
    }

    screenquad.changeG(G);
    screenquad2.changeG(G);
}

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);

    cube.Init();
    quad.Init();

    // setup view and projection matrices
    vec3 cam_pos(2.0f, 2.0f, 2.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    // create the model matrix (remember OpenGL is right handed)
    // accumulated transformation
    cube_model_matrix = scale(IDENTITY_MATRIX, vec3(0.5));
    cube_model_matrix = translate(cube_model_matrix, vec3(0.0, 0.0, 0.6));

    // on retina/hidpi displays, pixels != screen coordinates
    // this unsures that the framebuffer has the same size as the window
    // (see http://www.glfw.org/docs/latest/window.html#window_fbsize)
    glfwGetFramebufferSize(window, &window_width, &window_height);

    GLuint framebuffer_texture_id;
    GLuint framebuffer_texture_id_2;

    std::tie(framebuffer_texture_id, framebuffer_texture_id_2) =
            framebuffer.Init(window_width, window_height);

    //GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    screenquad.Init(window_width, window_height, framebuffer_texture_id);
    screenquad2.Init(window_width, window_height, framebuffer_texture_id_2, false);

    regenerateG();
}

void Display() {
    // render to framebuffer
    framebuffer.Clear();
    framebuffer.Bind(GL_COLOR_ATTACHMENT0);
    {
        cube.Draw(cube_model_matrix, view_matrix, projection_matrix);
        quad.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    }
    framebuffer.Unbind();

    // render to Window
    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer.Bind(GL_COLOR_ATTACHMENT1);
        screenquad.Draw();
    framebuffer.Unbind();

    screenquad2.Draw();
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    glViewport(0, 0, window_width, window_height);

    // when the window is resized, the framebuffer and the screenquad
    // should also be resized
    framebuffer.Cleanup();
    framebuffer.Init(window_width, window_height);
    screenquad.UpdateSize(window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if(gaussian_std > 0.3) {
            gaussian_std -= 0.25;
            regenerateG();
        }
    }
    if(key == GLFW_KEY_W && action == GLFW_PRESS) {
        if(gaussian_std < (MAX_SIZE-3)/12.0f - 1) {
            gaussian_std += 0.25;
            regenerateG();
        }
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "framebuffer", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);

    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    quad.Cleanup();
    cube.Cleanup();
    framebuffer.Cleanup();
    screenquad.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
