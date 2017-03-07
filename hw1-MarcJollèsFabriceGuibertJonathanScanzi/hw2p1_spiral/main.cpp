// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

#include <vector>

enum Type {
    Spiral, Fermat
};

constexpr Type TYPE_FIGURE = Spiral;

Triangle triangle;





void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

glm::mat4 rotation(float theta) {
    return glm::mat4(
                cos(theta), sin(theta), 0, 0,
               -sin(theta), cos(theta), 0, 0,
                0,          0,          1, 0,
                0,          0,          0, 1
                );
}

glm::mat4 translate(float tx, float ty = 0.0f) {
    return glm::mat4(
                1,  0,  0, 0,
                0,  1,  0, 0,
                0,  0,  1, 0,
                tx,  ty,  0, 1
                );
}

glm::mat4 scale(float s) {
    return glm::mat4(
                s,  0,  0, 0,
                0,  s,  0, 0,
                0,  0,  1, 0,
                0,  0,  0, 1
                );
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    size_t N;
    float theta, r, s;

    if(TYPE_FIGURE == Spiral) {
        N = 60;
    }
    else {
        N = 300;
    }


    for(size_t n = 0; n < N; ++n) {

        if(TYPE_FIGURE == Spiral) {
            theta = n/10.0 * M_PI; //for each triangle, we have 18° more than for the previous triangle.
            r = n/90.0;
            s = n/950.0 + 0.01; //0.01 is there so that even the first triangles can be seen
        }
        else {
            theta = n*137.508;
            r = 0.0045*sqrt(theta);
            s = 0.031;
        }

        triangle.Draw(rotation(theta)*translate(r)*scale(s)); // we scale first, then we translate to (r, 0), ans then we rotate according to the image center.
    }


}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

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

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    triangle.Cleanup();


    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

