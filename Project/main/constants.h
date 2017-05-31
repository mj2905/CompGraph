#pragma once

typedef uint8_t small_t;

#include "lightScattering/lightsource.h"
#define RENDER_DIM 1
const float FOG_THRESHOLD = 0.9;

const glm::vec3 MODEL_CENTER = glm::vec3(glm::translate(IDENTITY_MATRIX, glm::vec3(0.0f, -0.25f, -3.2)) * glm::scale(IDENTITY_MATRIX, glm::vec3(5,3,5)) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

#define SHADOW_MAP_RES 512

#define WATER_HEIGHT 1.43f

//camera switching
#define KEY_BEZIER_CAMERA GLFW_KEY_1
#define KEY_NORMAL_CAMERA GLFW_KEY_0
#define KEY_INERTIA_CAMERA GLFW_KEY_2
#define KEY_FPS_CAMERA GLFW_KEY_3



#define CAMERA_TYPE_BEZIER 0
#define CAMERA_TYPE_NORMAL 1
#define CAMERA_TYPE_INERTIA 2
#define CAMERA_TYPE_FPS 3

#define CAM_COUNT 4
