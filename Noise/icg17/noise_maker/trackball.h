#pragma once
#include "icg_helper.h"


using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) {}

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeingDrag(float x, float y) {
      anchor_pos_ = vec3(x, y, 0.0f);
      ProjectOntoSurface(anchor_pos_);
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
      //cout << x <<endl;
      vec3 current_pos = vec3(x, y, 0.0f);
      ProjectOntoSurface(current_pos);
      vec3 v1 = normalize(anchor_pos_);
      vec3 v2 = normalize(current_pos);
      vec3 N = cross(v1,v2);
      float angle = acos(dot(v1,v2));
      mat4 rotation = rotate(mat4(1.0f), angle, N);
      return rotation;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.

    float sq(float x) const{
        return x*x;
    }

    void ProjectOntoSurface(vec3 &p) const {
      p.x = p.x - anchor_pos_.x;
      p.y = p.y - anchor_pos_.y;


      float xposSquare = sq(p.x- anchor_pos_.x);
      float yposSquare = sq(p.y - anchor_pos_.y);
      float sqRadius = sq(radius_);


      if( xposSquare + yposSquare <= sqRadius/2){ // under hypothesis that z = 0, then we can ignore it in the equation
          p.z = sqrt(sqRadius - (xposSquare + yposSquare));
          cout << "p.z " << p.z << endl;
      } else {
          p.z = (sqRadius/2)*pow(xposSquare+yposSquare, -0.5);
      }
    }


    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
