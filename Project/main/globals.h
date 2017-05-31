#pragma once


// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "terrain/terrain.h"

// ----- camera and projection -----
int window_width = 800;
int window_height = 600;

glm::mat4 quad_model_matrix;
glm::mat4 global_view_matrix;
glm::vec3 global_position = glm::vec3(0.0f, -0.25f, -3.2);

bool switching_camera = false;
// ---------------------------------


// --------- FPS camera -------------
#define INCREMENT_STEPS (0.028f)
#define MULTITILES_INCREMENT (0.0001f)
#define WASDQE_NULL (0)
#define WASDQE_W (1)
#define WASDQE_A (2)
#define WASDQE_S (3)
#define WASDQE_D (4)
#define WASDQE_Q (5)
#define WASDQE_E (6)
#define WS_KEYS (0)
#define AD_KEYS (1)
#define QE_KEYS (2)
GLfloat global_angle_x = 0.528f;

uint8_t wasdqe_direction[3] = {WASDQE_NULL, WASDQE_NULL, WASDQE_NULL};
// ----------------------------------



glm::mat4 LookAtC(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    glm::vec3 z_cam = normalize(eye - center);
    glm::vec3 x_cam = normalize(cross(up, z_cam));
    glm::vec3 y_cam = cross(z_cam, x_cam);

    glm::mat3 R(x_cam, y_cam, z_cam);
    R = transpose(R);

    glm::mat4 look_at(glm::vec4(R[0], 0.0f),
                 glm::vec4(R[1], 0.0f),
                 glm::vec4(R[2], 0.0f),
                 glm::vec4(-R * (eye), 1.0f));
    return look_at;
}

//shadows
GLfloat near_plane = 1.0f, far_plane = 7.5f;
glm::vec3 light_pos = glm::vec3(2.0f, 0.0, 0.0);
glm::mat4 shadow_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//glm::mat4 shadow_view = glm::lookAt(light_pos,
//                                  glm::vec3( 0.0f, 0.0f,  0.0f),
//                                  glm::vec3( 0.0f, 1.0f,  0.0f));

glm::mat4 shadow_view = LookAtC(light_pos,
                      glm::vec3( 0.0f, 0.0f,  0.0f),
                      glm::vec3( 0.0f, 1.0f,  0.0f));

bool in_shadowmap = false;
GLuint shadowmap_fb;
#include "shadowmap/shadowmap.h"
shadowmap shadowmap;
