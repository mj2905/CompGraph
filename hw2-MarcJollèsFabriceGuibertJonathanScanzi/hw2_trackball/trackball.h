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
      vec3 current_pos = vec3(x, y, 0.0f);
      ProjectOntoSurface(current_pos);

      mat4 rotation = IDENTITY_MATRIX;
      // TODO 3: Calculate the rotation given the projections of the anocher
      // point and the current position. The rotation axis is given by the cross
      // product of the two projected points, and the angle between them can be
      // used as the magnitude of the rotation.
      // you might want to scale the rotation magnitude by a scalar factor.
      // p.s. No need for using complicated quaternions as suggested inthe wiki
      // article.

      const float SPEED = 2.0;
      vec3 v1 = normalize(anchor_pos_);
      vec3 v2 = normalize(current_pos);
      float angle = SPEED * acos(dot(v1,v2));

      rotation = rotate(mat4(1.0f), angle, cross(v1,v2));

      return rotation;
    }

private:

    float sq(float x) const {
        return x*x;
    }

    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
      // TODO 2: Implement this function. Read above link for details.
        float rSq = sq(radius_);
        float xSq = sq(p.x);
        float ySq = sq(p.y);

        if(xSq + ySq <= rSq) {
            p.z = sqrt(rSq - (xSq + ySq));
        }
        else {
            p.z = rSq/2 * pow(xSq + ySq, -0.5f);
        }

    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
