// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "arkanoid/ball.h"
#include "arkanoid/paddle.h"

constexpr float SPEED = 1;

Paddle paddle(/* width */ 0.3f, /* height */ 0.08f);
Ball ball(/* diameter */ 0.08f);


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if(key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        paddle.moveRight();
    }
    else if(key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        paddle.moveLeft();
    }
    else if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);
    paddle.Init();
    ball.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float time_s = glfwGetTime();
    float virtual_time_s = SPEED * time_s;

    paddle.Draw();
    ball.Draw();
}

void Update() {

    ball.Update();
    paddle.otherBounce(ball);

    if(ball.under(-0.7f)) {
        ball.Reset();
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "arkanoid", NULL, NULL);
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

    glfwSetKeyCallback(window, key_callback);

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        Update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    paddle.Cleanup();
    ball.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
