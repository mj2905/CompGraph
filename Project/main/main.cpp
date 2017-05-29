// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "globals.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "water/water.h"

#include "trackball.h"

#include "skybox/skybox.h"

#include "multitiles/multitiles.h"

#include "camera/abstractcamera.h"
#include "camera/beziercamera.h"
#include "camera/camera.h"
#include "camera/fps_camera.h"
#include "shadowmap/shadowmap.h"

#define WASD_NULL (0)
#define WASD_W (1)
#define WASD_A (2)
#define WASD_S (3)
#define WASD_D (4)

uint8_t wasd_direction[2] = {WASD_NULL, WASD_NULL};

#define INCREMENT_STEPS (0.028f)
#define MULTITILES_INCREMENT (0.0001f)




constexpr float NB_FPS = 60.0;


const unsigned int OFFSET_X = 256;
const unsigned int OFFSET_Y = 257;

MultiTiles multitiles(OFFSET_X, OFFSET_Y);

using namespace glm;

mat4 view_matrix;

mat4 projection_matrix;
mat4 quad_model_matrix;
mat4 quad_model_matrix_base;


float old_x, old_y;
GLfloat global_angle_x = 0.528f;
float distance_camera = 0;
AbstractCamera* current_cam;
Camera* regular_cam;
fps_camera* fps_cam;

Terrain* terrain;
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

    shadowmap.init();

    regular_cam = new Camera(multitiles);
    //camera = new BezierCamera({vec3(-1.9f, 2.25f, 0.65f), vec3(-2,0,-0.9), vec3(0,3.7,-2.3), vec3(1, 3.2, -4.5), vec3(2, 2, -6)}, {vec3(-1,0,-1), vec3(1,4,-2), vec3(2,2,-5)});
    current_cam = regular_cam;

    current_cam->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));

    quad_model_matrix = translate(IDENTITY_MATRIX, vec3(0.0f, -0.25f, -3.2)) * glm::scale(IDENTITY_MATRIX, vec3(5,3, 5));

    multitiles.Init(window_width, window_height);

    terrain = multitiles.getTerrain();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
vec3 up = vec3(0.0f, 1.0f, 0.0f);
vec3 position = vec3(0.0f, -0.25f, -3.2);
vec3 center = vec3(-1.0f, 1.1f, -1.2f);


void update_fps_cam() {

  //if WASD was pressed before the current frame
  if (wasd_direction[0] != WASD_NULL || wasd_direction[1] != WASD_NULL) {

    size_t factors;
    float angle_sin = sin(global_angle_x) * INCREMENT_STEPS;
    float angle_cos = cos(global_angle_x) * INCREMENT_STEPS;

    bool cos_pos = angle_cos > 0.0f;
    //angle_cos*=angle_cos;

    if (angle_sin <=0.0f) {

      angle_sin*=angle_sin;

      if (wasd_direction[1] == WASD_D) {
        multitiles.decrementY(angle_sin);
      }
      else if (wasd_direction[1] == WASD_A) {
        multitiles.incrementY(angle_sin);
      }

      if (wasd_direction[0] == WASD_W) {
        multitiles.incrementX(angle_sin);
      }
      else if (wasd_direction[0] == WASD_S) {
        multitiles.decrementX(angle_sin);
      }
    }

    else {
      angle_sin*=angle_sin;

      if (wasd_direction[1] == WASD_D) {
        multitiles.incrementY(angle_sin);
      }
      else if (wasd_direction[1] == WASD_A) {
        multitiles.decrementY(angle_sin);
      }


      if (wasd_direction[0] == WASD_W) {
        multitiles.decrementX(angle_sin);
      }
      else if (wasd_direction[0] == WASD_S) {
        multitiles.incrementX(angle_sin);
      }

    }

    if (angle_cos <=0.0f) {
      angle_cos*=angle_cos;

      if (wasd_direction[1] == WASD_D) {
        multitiles.incrementX(angle_cos);
      }
      else if (wasd_direction[1] == WASD_A) {
        multitiles.decrementX(angle_cos);
      }


      if (wasd_direction[0] == WASD_W) {
        multitiles.incrementY(angle_cos);
      }
      else if (wasd_direction[0] == WASD_S) {
        multitiles.decrementY(angle_cos);
      }
    }


    else {
      angle_cos*=angle_cos;

      if (wasd_direction[1] == WASD_D) {
        multitiles.decrementX(angle_cos);
      }
      else if (wasd_direction[1] == WASD_A) {
        multitiles.incrementX(angle_cos);
      }

      if (wasd_direction[0] == WASD_W) {
        multitiles.decrementY(angle_cos);
      }
      else if (wasd_direction[0] == WASD_S) {
        multitiles.incrementY(angle_cos);
      }
    }


  }
}

// gets called for every frame.
void Display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);

    position.y = terrain->getCurrentHeight()*2;

    view_matrix = LookAtC(position, center, up);

    //handling FPS camera
    update_fps_cam();


    //generating real-time shadowmap
    shadowmap.bind(quad_model_matrix);
    multitiles.Draw(quad_model_matrix, current_cam->getView(), shadow_projection);
    shadowmap.unbind();

    multitiles.Draw(quad_model_matrix, current_cam->getView(), projection_matrix);

    current_cam->animate();


}

bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;

void Update() {
    //multitiles.incrementY(); //to move with the camera

    float increment = 0.05f;

    if(upPressed and not downPressed) {
        current_cam->move(0, 0, increment);
    }
    if(downPressed and not upPressed) {
        current_cam->move(0, 0, -increment);
    }
    if(leftPressed and not rightPressed) {
        current_cam->move(increment, 0, 0);
    }
    if(rightPressed and not leftPressed) {
        current_cam->move(-increment, 0, 0);
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

//mat4 view_matrix;

const float MIN_DISTANCE_POLE = 0.1f;

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

        //fps
        float delta_x = old_x - p.x;
        global_angle_x += old_x - p.x;
        float delta_y = p.y - old_y;

        //new stuff
        vec3 angle_x = normalize(cross(up, -center + position));
        vec3 angle_y = normalize(cross(angle_x, -center+position));
        vec3 oldCenter = center;
        center = (mat3(glm::rotate(glm::rotate(IDENTITY_MATRIX, delta_x, angle_y), delta_y, angle_x)) * (center - position)) + position;



        //normal
        current_cam->rotate(old_x - p.x, p.y - old_y);


        old_x = p.x;
        old_y = p.y;
   }
}



// Gets called when the windows/framebuffer is resized.
void SetupProjection(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    // TODO 1: Use a perspective projection instead;

    projection_matrix = PerspectiveProjection(45.0f,
                                              (GLfloat)window_width / window_height,
                                              0.1f, 100.0f);

  //  shadow_projection = OrthographicProjection(-20.0, 20.0, -20.0, 20.0,
  //                                            0.01f, 1000.0f);



    multitiles.Cleanup();
    multitiles.Init(width, height);

    //GLfloat top = 1.0f;
    //GLfloat right = (GLfloat)window_width / window_height * top;
    //projection_matrix = OrthographicProjection(-right, right, -top, top, -10.0, 10.0f);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
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


        //rest

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
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        current_cam->increaseVelocity();
    }
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        current_cam->decreaseVelocity();
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

    //vector<vec3> points = {vec3(0), vec3(1, 1, 0), vec3(2, 0, 0), vec3(3, 1, 0), vec3(4, 0, 0)};
    //Bezier bezier(points);
    //float t = 0.2;
    //cout << bezier.apply(t).x << " " << bezier.apply(t).y << " " << bezier.apply(t).z << endl;

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
            Display();
            Update();
            lastTime = time;
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }

    multitiles.Cleanup();
    delete current_cam;


    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
