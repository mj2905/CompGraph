// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "constants.h"
#include "globals.h"

#include "water/water.h"

#include "trackball.h"

#include "skybox/skybox.h"

#include "multitiles/multitiles.h"

#include "camera/abstractcamera.h"
#include "camera/beziercamera.h"
#include "camera/camera.h"
#include "camera/inertiacamera.h"
#include "camera/fps_camera.h"

#include "shadowmap/shadowmap.h"

constexpr float NB_FPS = 60.0;


const unsigned int OFFSET_X = 256;
const unsigned int OFFSET_Y = 257;

MultiTiles multitiles(OFFSET_X, OFFSET_Y);

using namespace glm;

mat4 projection_matrix;

LightSource light;
AbstractCamera* camera;
AbstractCamera* cams[CAM_COUNT];


Terrain* terrain;

float old_x, old_y;

mat4 OrthographicProjection(float left, float right, float bottom,
                            float top, float near, float far) {
    assert(right > left);
    assert(far > near);
    assert(top > bottom);
    mat4 projection = mat4(1.0f);
    projection[0][0] = 2.0f / (right - left);
    projection[1][1] = 2.0f / (top - bottom);
    projection[2][2] = -2.0f / (far - near);
    projection[3][3] = 1.0f;
    projection[3][0] = -(right + left) / (right - left);
    projection[3][1] = -(top + bottom) / (top - bottom);
    projection[3][2] = -(far + near) / (far - near);
    return projection;
}

mat4 PerspectiveProjection(float fovy, float aspect, float near, float far) {
    // TODO 1: Create a perspective projection matrix given the field of view,
    // aspect ratio, and near and far plane distances.

    fovy *= M_PI/180.0f;

    mat4 pro = mat4(0);
    pro[0][0] = 1.0/(aspect*tan(fovy)); // near/right = near/(top*aspect) = near/(near*tan(fovy/2)*aspect) = 1/(tan(fovy/2) * aspect)
    pro[1][1] = 1.0/tan(fovy); // near/top = near/(near*tan(fovy/2)) = 1 / tan(fovy/2)
    pro[3][2] = -2.0*far*near/(far - near); // no simplification possible here :(
    pro[2][2] = -1.0*(far+near)/(far-near); // again, nothing to be simplified :(
    pro[2][3] = -1.0;
    return pro; // we already transposed the matrix (since it's column major!) so no need to transpose it again
}

mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
    // we need a function that converts from world coordinates into camera coordiantes.
    //
    // cam coords to world coords is given by:
    // X_world = R * X_cam + eye
    //
    // inverting it leads to:
    //
    // X_cam = R^T * X_world - R^T * eye
    //
    // or as a homogeneous matrix:
    // [ r_00 r_10 r_20 -r_0*eye
    //   r_01 r_11 r_21 -r_1*eye
    //   r_02 r_12 r_22 -r_2*eye
    //      0    0    0        1 ]

    vec3 z_cam = normalize(eye - center);
    vec3 x_cam = normalize(cross(up, z_cam));
    vec3 y_cam = cross(z_cam, x_cam);

    mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    mat4 look_at(vec4(R[0], 0.0f),
            vec4(R[1], 0.0f),
            vec4(R[2], 0.0f),
            vec4(-R * (eye), 1.0f));
    return look_at;
}

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // TODO 3: once you use the trackball, you should use a view matrix that
    // looks straight down the -z axis. Otherwise the trackball's rotation gets
    // applied in a rotated coordinate frame.
    // uncomment lower line to achieve this.
    /*view_matrix = LookAt(vec3(2.0f, 2.0f, 2.0f),
                         vec3(0.0f, 0.0f, 0.0f),
                         vec3(0.0f, 1.0f, 0.0f));*/
    //view_matrix = translate(IDENTITY_MATRIX, vec3(0.0f, -2.0f, distance_camera)) * glm::rotate(IDENTITY_MATRIX, (float)M_PI/4.0f, vec3(1, 0, 0));





    quad_model_matrix = translate(IDENTITY_MATRIX, vec3(0.0f, -0.25f, -3.2)) * glm::scale(IDENTITY_MATRIX, vec3(5,3, 5));

    vec3 light_init = vec3(1.0,1.0,-1.0); // NOTE: IT IS NOT ALIGNED WITH THE SUN OF THE SKYBOX
    light.Init(light_init.x, light_init.y, light_init.z);

    multitiles.Init(window_width, window_height, light);

    terrain = multitiles.getTerrain();

    cams[CAMERA_TYPE_NORMAL] = new Camera(multitiles);
    cams[CAMERA_TYPE_INERTIA] = new InertiaCamera();
    //camera = new BezierCamera({vec3(-1.9f, 2.25f, 0.65f), vec3(-2,0,-0.9), vec3(0,3.7,-2.3), vec3(1, 3.2, -4.5), vec3(2, 2, -6)}, {vec3(-1,0,-1), vec3(1,4,-2), vec3(2,2,-5)});
    cams[CAMERA_TYPE_FPS] = new class fps_camera(*terrain, multitiles);

    camera = cams[CAMERA_TYPE_NORMAL];
    camera->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

vec3 up = vec3(0.0f, 1.0f, 0.0f);

vec3 center = vec3(-1.0f, 1.1f, -1.2f);




// gets called for every frame.
void Display() {

    camera->animate();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);


    //generating real-time shadowmap

    //shadowmap.bind(quad_model_matrix);
    //multitiles.Draw(quad_model_matrix, camera->getView(), shadow_projection);
    //shadowmap.unbind();

    multitiles.Draw(quad_model_matrix, camera->getView(), projection_matrix,1);
}

bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;

void Update() {
    //multitiles.incrementY(); //to move with the camera

    float increment = 0.05f;

    if(upPressed and not downPressed) {
        camera->move(0, 0, increment);
    }
    if(downPressed and not upPressed) {
        camera->move(0, 0, -increment);
    }
    if(leftPressed and not rightPressed) {
        camera->move(increment, 0, 0);
    }
    if(rightPressed and not leftPressed) {
        camera->move(-increment, 0, 0);
    }
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        // Store the current state of the model matrix.
    }
    old_y = -2;
    old_x = -2;
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // TODO 3: Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        // trackball_matrix = ...
        //view_matrix = trackball.Drag(p.x, p.y) * view_matrix;

        if(old_x < -1.5) {
            old_x = p.x;
        }
        if(old_y < -1.5) {
            old_y = p.y;
        }

        camera->rotate(old_x - p.x, p.y - old_y);

        old_x = p.x;
        old_y = p.y;
    }
}

// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {

    bool modified = window_width != width || window_height != height;
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    // TODO 1: Use a perspective projection instead;
    projection_matrix = PerspectiveProjection(45.0f,
                                              (GLfloat)window_width / window_height,
                                              0.1f, 100.0f);
    if(modified) {
        multitiles.Cleanup();
        multitiles.Init(width, height, light);
    }

}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  if (!switching_camera) {


    //switching camera
    if (key == KEY_NORMAL_CAMERA && camera->type_of_camera() != CAMERA_TYPE_NORMAL) {
      switching_camera = true;
      delete cams[camera->type_of_camera()];
      cams[CAMERA_TYPE_NORMAL] = new Camera(multitiles);
      camera = cams[CAMERA_TYPE_NORMAL];
      camera->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));
      switching_camera = false;
    }

    else if (key == KEY_INERTIA_CAMERA && camera->type_of_camera() != CAMERA_TYPE_INERTIA) {
      switching_camera = true;
      delete cams[camera->type_of_camera()];
      cams[CAMERA_TYPE_INERTIA] = new InertiaCamera();
      camera = cams[CAMERA_TYPE_INERTIA];
      camera->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));
      switching_camera = false;

    }
    else if (key == KEY_FPS_CAMERA && camera->type_of_camera() != CAMERA_TYPE_FPS) {
      switching_camera = true;
      delete cams[camera->type_of_camera()];
      cams[CAMERA_TYPE_FPS] = new class fps_camera(*terrain, multitiles);
      camera = cams[CAMERA_TYPE_FPS];
      camera->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));
      switching_camera = false;
    }

    //FPS
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
      //view_matrix = translate(IDENTITY_MATRIX, vec3(0, 0, 0.1)) * view_matrix;
      wasd_direction[0] = WASD_W;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
      wasd_direction[0] = WASD_NULL;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      //view_matrix = translate(IDENTITY_MATRIX, vec3(0, 0, 0.1)) * view_matrix;
      wasd_direction[0] = WASD_S;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
      wasd_direction[0] = WASD_NULL;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      //view_matrix = translate(IDENTITY_MATRIX, vec3(0, 0, 0.1)) * view_matrix;
      wasd_direction[1] = WASD_A;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
      wasd_direction[1] = WASD_NULL;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      //view_matrix = translate(IDENTITY_MATRIX, vec3(0, 0, 0.1)) * view_matrix;
      wasd_direction[1] = WASD_D;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
      wasd_direction[1] = WASD_NULL;
    }


    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginFwAccel();
    }
    if(key == GLFW_KEY_K &&(action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginBwAccel();
    }
    if(key == GLFW_KEY_Q &&(action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginYawAccel();
    }
    if(key == GLFW_KEY_E &&(action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginReverseYawAccel();

    }
    if(key == GLFW_KEY_J &&(action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginPitchAccel();

    }
    if(key == GLFW_KEY_L &&(action == GLFW_PRESS || action == GLFW_REPEAT)){
      camera->beginReversePitchAccel();

    }
    if (key == GLFW_KEY_UP) {
      if(action == GLFW_PRESS) {
        upPressed = true;
      }
      else if(action == GLFW_RELEASE) {
        upPressed = false;
      }
    }

    if (key == GLFW_KEY_DOWN) {
      if(action == GLFW_PRESS) {
        downPressed = true;
      }
      else if(action == GLFW_RELEASE) {
        downPressed = false;
      }
    }

    if (key == GLFW_KEY_LEFT) {
      if(action == GLFW_PRESS) {
        leftPressed = true;
      }
      else if(action == GLFW_RELEASE) {
        leftPressed = false;
      }
    }

    if (key == GLFW_KEY_RIGHT) {
      if(action == GLFW_PRESS) {
        rightPressed = true;
      }
      else if(action == GLFW_RELEASE) {
        rightPressed = false;
      }
    }
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera->increaseVelocity();
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera->decreaseVelocity();
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
                                          "Mountains", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, SetupProjection);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
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

    constexpr double limitSPF = 1.0/NB_FPS;
    double lastTime = 0, time = 0;

    // render loop
    while(!glfwWindowShouldClose(window)){

        time = glfwGetTime();
        if(time - lastTime >= limitSPF) {
          Update();
            Display();

            lastTime = time;
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }

    multitiles.Cleanup();
    delete camera;


    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
