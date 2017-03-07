// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

constexpr float SPEED = 1; //in days

Quad sun, earth, moon;

constexpr float X_CENTER_ELLIPSIS = -0.1f;
constexpr float Y_CENTER_ELLIPSIS = 0;

constexpr float SUN_SIZE = 0.45f;
constexpr float EARTH_SIZE = 0.15f;
constexpr float MOON_SIZE = 0.07f;

constexpr float DISTANCE_EARTH_MOON = 0.15f;

constexpr float A_ELLIPSE = 0.7f; //width of ellipsis
constexpr float B_ELLIPSE = 0.5f; //height of ellipsis

constexpr float X_CENTER_TO_FOCUS = 0.25f; //distance between center of ellipsis and sun (focus)
constexpr float Y_CENTER_TO_FOCUS = 0; //distance between center of ellipsis and sun (focus)

constexpr float NB_ROTATIONS_EARTH_YEAR = 365;
constexpr float NB_DAYS_ROTATION_MOON = 27;
constexpr float NB_DAYS_ROTATION_SUN = 24.47f;

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
    float sun_time_s = SPEED * time_s;
    float earth_time_s = SPEED * time_s;
    float moon_time_s = SPEED * time_s;

    float sun_omega = fmod(sun_time_s, NB_DAYS_ROTATION_SUN) * 2 * M_PI / NB_DAYS_ROTATION_SUN;
    float earth_rot_omega = fmod(sun_time_s, NB_ROTATIONS_EARTH_YEAR) * 2 * M_PI / NB_ROTATIONS_EARTH_YEAR;
    float earth_omega = fmod(earth_time_s, 1) * 2 * M_PI;

    float moon_omega = fmod(moon_time_s, NB_DAYS_ROTATION_MOON) * 2 * M_PI / NB_DAYS_ROTATION_MOON;

    float x_earth = A_ELLIPSE*cos(-earth_rot_omega) + X_CENTER_ELLIPSIS;
    float y_earth = B_ELLIPSE*sin(-earth_rot_omega) + Y_CENTER_ELLIPSIS;

    float x_moon = DISTANCE_EARTH_MOON * cos(moon_omega);
    float y_moon = DISTANCE_EARTH_MOON * sin(moon_omega);


    glm::mat4 sun_M = IDENTITY_MATRIX;
    glm::mat4 earth_M = sun_M * translate(x_earth, y_earth);
    glm::mat4 moon_M = earth_M * translate(x_moon, y_moon);


    sun.Draw(sun_M * translate(X_CENTER_ELLIPSIS + X_CENTER_TO_FOCUS, Y_CENTER_ELLIPSIS + Y_CENTER_TO_FOCUS)
                   * rotation(sun_omega) * scale(SUN_SIZE/2.0f));

    earth.Draw(earth_M * rotation(earth_omega) * scale(EARTH_SIZE/2.0f));

    moon.Draw(moon_M * rotation(moon_omega) * scale(MOON_SIZE/2.0f));
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
