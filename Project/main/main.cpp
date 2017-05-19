// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "constants.h"
// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "water/water.h"

#include "trackball.h"

#include "skybox/skybox.h"

#include "multitiles/multitiles.h"

#include "camera/abstractcamera.h"
#include "camera/beziercamera.h"
#include "camera/camera.h"


constexpr float NB_FPS = 60.0;


const unsigned int OFFSET_X = 256;
const unsigned int OFFSET_Y = 257;

MultiTiles multitiles(OFFSET_X, OFFSET_Y);
LightSource light;

int window_width = 800;
int window_height = 600;
int fboId, fboTexId;

float uniformLightX ;
float uniformLightY ;

using namespace glm;

mat4 projection_matrix;

mat4 quad_model_matrix;
mat4 quad_model_matrix_base;
FrameBuffer frameBuffer;

AbstractCamera* camera;

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

    light.Init(0.0,1.0,-1.0);

    camera = new Camera(multitiles);
    //camera = new BezierCamera({vec3(-1.9f, 2.25f, 0.65f), vec3(-2,0,-0.9), vec3(0,3.7,-2.3), vec3(1, 3.2, -4.5), vec3(2, 2, -6)}, {vec3(-1,0,-1), vec3(1,4,-2), vec3(2,2,-5)});

    camera->Init(vec3(-2, 1.3, 1), vec3(-1.0f, 1.1f, -1.2f), vec3(0.0f, 1.0f, 0.0f));

    quad_model_matrix = translate(IDENTITY_MATRIX, vec3(0.0f, -0.25f, -3.2)) * glm::scale(IDENTITY_MATRIX, vec3(5,3, 5));

    multitiles.Init(window_width, window_height, light);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}



void ScatterDisplay(GLFWwindow* window){
    int renderWidth, renderHeight;
    double modelView[16]={quad_model_matrix[0][0], quad_model_matrix[0][1], quad_model_matrix[0][2],quad_model_matrix[0][3],
                         quad_model_matrix[1][0], quad_model_matrix[1][1], quad_model_matrix[1][2],quad_model_matrix[1][3],
                         quad_model_matrix[2][0], quad_model_matrix[2][1], quad_model_matrix[2][2],quad_model_matrix[2][3],
                         quad_model_matrix[3][0], quad_model_matrix[3][1], quad_model_matrix[3][2],quad_model_matrix[3][3]};

    double projection[16] = {
        projection_matrix[0][0], projection_matrix[0][1], projection_matrix[0][2], projection_matrix[0][3],
        projection_matrix[1][0], projection_matrix[1][1], projection_matrix[1][2], projection_matrix[1][3],
        projection_matrix[2][0], projection_matrix[2][1], projection_matrix[2][2], projection_matrix[2][3],
        projection_matrix[3][0], projection_matrix[3][1], projection_matrix[3][2], projection_matrix[3][3],
    };

    GLint viewport[4];
    double depthRange[2];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_DEPTH_RANGE, depthRange);


    glfwGetFramebufferSize(window,&renderWidth, &renderHeight);



    // render light + obstruction
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
    glClear ( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    glColor4f(1,1,1,1);
    glViewport(0,0,window_width/*/OFF_SCREEN_RENDER_RATIO*/,window_height/*/OFF_SCREEN_RENDER_RATIO*/);
    glDisable(GL_TEXTURE_2D);
        glPushMatrix();
            //glTranslatef(light->lightPosition[0],light->lightPosition[1],light->lightPosition[2]);
            glTranslatef(light.getPosition().at(0), light.getPosition().at(1), light.getPosition().at(2));
            //light->render();
        glPopMatrix();
        GLdouble winX=0, winY =0, winZ = 0;

        gluProject(light.getPosition().at(0), light.getPosition().at(1), light.getPosition().at(2),
                        modelView,
                        projection,
                        viewport,
                        &winX,
                        &winY,
                        &winZ);


            uniformLightX = winX/((float)renderWidth/*/OFF_SCREEN_RENDER_RATIO*/);
            uniformLightY = winY/((float)renderHeight/*/OFF_SCREEN_RENDER_RATIO*/) ;

/*
    // Draw occluding source black with light
                glUseProgramObjectARB(0);


                glEnable(GL_TEXTURE_2D);
                glColor4f(0,0,0,1);
                glPushMatrix();
                    tenso->render();
                glPopMatrix();

               glPushMatrix();
                    glTranslatef(ikal->position.x,ikal->position.y,ikal->position.z);
                    glRotatef(180,0,1,0);
                    glRotatef(ikal->eulerRotation.z,0,0,1);
                    ikal->render();
               glPopMatrix();
               // Make sky a bit more visible
               //glColor4f(0.19f,0.19f,0.19f,1);
               //sky1->render();
              // glPushMatrix();
            //		glRotatef(180,1,0,0);
            //		sky1->render();
            //	glPopMatrix();


            // Save screen or Switch to normal rendering
            if (fboUsed)
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            else
                copyFrameBufferToTexture();

    glViewport(0,0,renderWidth,renderHeight);
    glEnable(GL_TEXTURE_2D);

  /*
    // Render the scene with no light scattering

    glClear (GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

                glColor4f(1,1,1,1);
                glPushMatrix();
                    tenso->render();
               glPopMatrix();

               glPushMatrix();
                    glTranslatef(ikal->position.x,ikal->position.y,ikal->position.z);
                    glRotatef(180,0,1,0);
                    glRotatef(ikal->eulerRotation.z,0,0,1);
                    ikal->render();
                glPopMatrix();

               //sky1->render();
               glPushMatrix();
                    glRotatef(180,1,0,0);
                    sky1->render();
                glPopMatrix();

        //glDisable(GL_BLEND);


        // Paint the light scaterring effect

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho( -renderWidth/2,renderWidth/2, -renderHeight/2, renderHeight/2, 000, 50000.0 );
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glClear (GL_DEPTH_BUFFER_BIT );



                glActiveTextureARB(GL_TEXTURE0_ARB);
                if (!fboUsed)
                    glBindTexture(GL_TEXTURE_2D, screenCopyTextureId);
                else
                    glBindTexture(GL_TEXTURE_2D, fboTextureId);



                glUseProgramObjectARB(shader);
                glUniform2fARB(glsl_loc_light,uniformLightX,uniformLightY);
                glUniform1fARB(glsl_loc_exposure,uniformExposure);
                glUniform1fARB(glsl_loc_decay,uniformDecay);
                glUniform1fARB(glsl_loc_density,uniformDensity);
                glUniform1fARB(glsl_loc_weight,uniformWeight);
                glUniform1iARB(glsl_loc_myTexture,0);

                glEnable(GL_TEXTURE_2D);
                glEnable(GL_BLEND);

                glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                 glBegin(GL_QUADS);
                         glTexCoord2f(0,0);
                         glVertex2f(-renderWidth/2,-renderHeight/2);

                        glTexCoord2f(1,0);
                        glVertex2f(renderWidth/2,-renderHeight/2);

                        glTexCoord2f(1,1);
                        glVertex2f(renderWidth/2,renderHeight/2);

                        glTexCoord2f(0,1);
                        glVertex2f(-renderWidth/2,renderHeight/2);
                glEnd();


        glUseProgramObjectARB(0);

        // DRAWING TEXT
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho( -renderWidth/2,renderWidth/2, -renderHeight/2, renderHeight/2, 00.1f, 600.0f );
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslated(0,0,-500);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glClear ( GL_DEPTH_BUFFER_BIT );
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        sprintf(fps,"fps: %.0f",Timer::fps );
        sprintf(polyCount,"polygones rendered: %d",polygonRendered );
        sprintf(s_textSwitchs,"textures switches: %d",textureSwitchs );
        drawString(fps,renderWidth/2-250,-renderHeight/2+60);
        drawString(polyCount,renderWidth/2-250,-renderHeight/2+40);

        drawString(s_textSwitchs,renderWidth/2-250,-renderHeight/2+20);
        //drawString(s_extensions,-renderWidth/2+10,renderHeight/2-80);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
       // END DRAWING TEXT
*/

}

// gets called for every frame.
void Display() {
    //
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);

    multitiles.Draw(quad_model_matrix, camera->getView(), projection_matrix);

    camera->animate();

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
    window_width = width;
    window_height = height;

    cout << "Window has been resized to "
         << window_width << "x" << window_height << "." << endl;

    glViewport(0, 0, window_width, window_height);

    // TODO 1: Use a perspective projection instead;
    projection_matrix = PerspectiveProjection(45.0f,
                                              (GLfloat)window_width / window_height,
                                              0.1f, 100.0f);

    multitiles.Cleanup();
    multitiles.Init(width, height, light);

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
            ScatterDisplay(window);
            Display();
            Update();
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
