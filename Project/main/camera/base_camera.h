#pragma once

#include "icg_helper.h"

using namespace glm;

class base_camera {
   public:
      // pure virtual function
      virtual void update_rot(glm::mat4& rotx, glm::mat4& roty, double x, double y, double old_x, double old_y) = 0;
   private:
     base_camera();


};
