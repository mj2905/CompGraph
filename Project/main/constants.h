#pragma once
#include "lightScattering/lightsource.h"
#define RENDER_DIM 4
const float FOG_THRESHOLD = 0.9;

const glm::vec3 MODEL_CENTER = glm::vec3(glm::translate(IDENTITY_MATRIX, glm::vec3(0.0f, -0.25f, -3.2)) * glm::scale(IDENTITY_MATRIX, glm::vec3(5,3,5)) * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

#define SHADOW_MAP_RES 512
