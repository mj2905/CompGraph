// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "mesh/mesh.h"
#include "quad/quad.h"
#include "framebuffer_scattering.h"
#include "screenquad/screenquad.h"
#include <glm/gtc/type_ptr.hpp>

int window_width = 1024;
int window_height = 768;
Mesh mesh;
Quad quad, quad2, quad3;
FrameBufferScattering framebuffer, fbo2;
ScreenQuad screenquad;

glm::mat4 projection_matrix;
glm::mat4 view_matrix;

void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    glViewport(0, 0, window_width, window_height);
    float ratio = window_width / (float) window_height;
    projection_matrix = glm::perspective(45.0f, ratio, 0.1f, 10.0f);
}

void Init() {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);


    int frameId1, frameId2;
    frameId1 = framebuffer.Init(window_width, window_height, true);
    frameId2 = fbo2.Init(window_width, window_height);

    mesh.Init("sphere.obj");
    quad.Init();
    quad2.Init();
    quad3.Init();
    screenquad.Init(window_width, window_height, frameId1,frameId2, true);

    // setup view matrix
    glm::vec3 cam_pos(2.0f, 2.0f, 2.0f);
    glm::vec3 cam_look(0.0f, 0.0f, 0.0f);
    glm::vec3 cam_up(0.0f, 0.0f, 1.0f);
    view_matrix = glm::lookAt(cam_pos, cam_look, cam_up);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 trans= glm::translate(glm::mat4(1.0f),glm::vec3(0.1,0.8,0.0));
    glm::mat4 trans2= glm::translate(glm::mat4(1.0f),glm::vec3(0.8,0.7,0.0));
    glm::mat4 trans3= glm::translate(glm::mat4(1.0f),glm::vec3(-0.8,-0.3,0.0));



    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer.Clear();
    fbo2.Clear();


    framebuffer.Bind();{
        quad.Draw(IDENTITY_MATRIX, view_matrix, trans*projection_matrix,0);
        quad2.Draw(IDENTITY_MATRIX, view_matrix, trans2*projection_matrix,0);
        quad3.Draw(IDENTITY_MATRIX, view_matrix, trans3*projection_matrix,0);

    }
    framebuffer.Unbind();


    fbo2.Bind();{
        quad.Draw(IDENTITY_MATRIX, view_matrix, trans*projection_matrix,1);
        quad2.Draw(IDENTITY_MATRIX, view_matrix, trans2*projection_matrix,1);
        quad3.Draw(IDENTITY_MATRIX, view_matrix, trans3*projection_matrix,1);
        mesh.Draw(IDENTITY_MATRIX, view_matrix, projection_matrix);
    }
    framebuffer.Unbind();




    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screenquad.Draw();


}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    // only act on release
    if(action != GLFW_RELEASE) {
        return;
    }
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
glm::vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return glm::vec2(2.0f * (float)x / width - 1.0f,
                     1.0f - 2.0f * (float)y / height);
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        glm::vec2 p = TransformScreenCoords(window, x_i, y_i);
        mesh.light_pos = glm::vec3(-p.x, -p.y, 2.0f);
        // Store the current state of the model matrix.
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
                                          "Shading", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);
    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);
    // set the mouse press and position callback
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    SetupProjection(window, window_width, window_height);

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mesh.Cleanup();
    quad.Cleanup();
    quad2.Cleanup();
    screenquad.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
