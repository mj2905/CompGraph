#pragma once

// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

// contains helper functions such as shader compiler
#include "icg_helper.h"
#include "shadowmap/shadowmap.h"

shadowmap global_shadowmap;
glm::mat4 shadow_projection;
glm::mat4 shadow_view;
