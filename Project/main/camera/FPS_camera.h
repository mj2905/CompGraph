#pragma once

#include "icg_helper.h"
using namespace glm;

class fps_camera {
   public:
      // pure virtual function
      base_camera() {

      }
      virtual void update_rot(glm::mat4& rotx, glm::mat4& roty, double x, double y, double old_x, double old_y) = 0;
   private:

};
