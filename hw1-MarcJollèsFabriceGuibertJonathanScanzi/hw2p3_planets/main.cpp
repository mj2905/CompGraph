// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

constexpr float SPEED = 1;

Quad sun, earth, moon;

constexpr float X_SUN = 0.25f;
constexpr float Y_SUN = 0;

constexpr float SUN_SIZE = 0.45f;
constexpr float EARTH_SIZE = 0.15f;
constexpr float MOON_SIZE = 0.07f;

constexpr float DISTANCE_EARTH_MOON = 0.15f;

constexpr float A_ELLIPSE = 0.8f;
constexpr float B_ELLIPSE = 0.55f;

constexpr float X_CENTER_TO_FOCUS = 0.3f;
constexpr float Y_CENTER_TO_FOCUS = 0;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);
    sun.Init("sun.tga");
    earth.Init("earth.tga");
    moon.Init("moon.tga");
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
    float time_s = glfwGetTime();
    float virtual_time_s = SPEED * time_s;

    float sun_angle = fmod(virtual_time_s, 2*M_PI);
    float earth_angle = fmod(2*virtual_time_s, 2*M_PI);
    float moon_angle = earth_angle;

    float x = A_ELLIPSE*cos(-sun_angle) - X_CENTER_TO_FOCUS;
    float y = B_ELLIPSE*sin(-sun_angle) - Y_CENTER_TO_FOCUS;



    glm::mat4 sun_M = translate(X_SUN, Y_SUN);
    glm::mat4 earth_M = sun_M*translate(x,y) * rotation(earth_angle);
    glm::mat4 moon_M = earth_M * translate(DISTANCE_EARTH_MOON) * rotation(moon_angle);


    sun.Draw(sun_M * rotation(sun_angle) * scale(SUN_SIZE/2.0f));

    earth.Draw(earth_M* scale(EARTH_SIZE/2.0f));

    moon.Draw(moon_M* scale(MOON_SIZE/2.0f));
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
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

    sun.Cleanup();
    earth.Cleanup();
    moon.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
