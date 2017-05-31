#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "abstractcamera.h"
#include "../multitiles/multitiles.h"



#define INFINITE_TERRAIN true


using namespace glm;

class fps_camera2 : public AbstractCamera {

private:

    const float MIN_DISTANCE_POLE = 0.1f;
    Terrain& terrain;


    inline void clamp(GLfloat min, GLfloat max, GLfloat& val) {
      if (val < min) {
        val = min;
      }
      else if (val > max) {
        val = max;
      }
    }




public:

    fps_camera2(Terrain& terrain): AbstractCamera(), terrain(terrain) {}

    virtual ~fps_camera2() {}

    virtual void animate() override {}

    void rotate(float x, float z) override {

        vec3 xpivot = normalize(cross(up, center - position));
        vec3 ypivot = normalize(cross(xpivot, center-position));

        vec3 tmp = (mat3(glm::rotate(IDENTITY_MATRIX, x, ypivot)) * (center - position)) + position;

        if(fmod(angle(normalize(tmp - position), up), M_PI - MIN_DISTANCE_POLE) < MIN_DISTANCE_POLE) {
            tmp = center;
        }

        center = (mat3(glm::rotate(IDENTITY_MATRIX, z, xpivot)) * (tmp-position)) + position;

        if(fmod(angle(normalize(center - position), up), M_PI - MIN_DISTANCE_POLE) < MIN_DISTANCE_POLE) {
            center = tmp;
        }

        view_matrix = LookAt(position, center, up);
        global_view_matrix = LookAt(position, center, up);

    }

    virtual void move(float x, float y, float z) override {

      vec3 tmp = center - position;
      tmp.y = 0;

      vec3 zpivot = normalize(tmp);
      vec3 xpivot = normalize(cross(up, zpivot));

      vec3 oldPosition = position;

      position += zpivot * z + xpivot * x;
      center += xpivot * x + zpivot * z + up*y;

      clamp(-4.5, 4.5, position[0]);
      clamp(-4.5, 4.5, position[2]);

      const GLfloat x_percent =  ((position.x + 5.0) / 10.0);
      const GLfloat z_percent = 1.0 - ((position.z + 5.0) / 10.0);

      const GLuint fb_width  = x_percent * window_width;
      const GLuint fb_height = z_percent * window_height;

      //cout << "position " << position.x << " " << position.y << " " << position.z << endl;
      //cout << "center " << center.x << " " << center.y << " " << center.z << endl;

      GLfloat height_r = terrain.getCurrentHeight(fb_width, fb_height)*3 + 0.37;
      cout << "height" << height_r << endl;




      position.y = height_r;
      view_matrix = LookAt(position, center, up);
      global_view_matrix = LookAt(position, center, up);

    }

    virtual void increaseVelocity() override {}
    virtual void decreaseVelocity() override {}

    virtual void beginFwAccel() override {}
    virtual void beginBwAccel() override{}

    virtual void beginYawAccel() override{}
    virtual void beginReverseYawAccel() override{}

    virtual void beginPitchAccel() override{}
    virtual void beginReversePitchAccel() override{}

    virtual small_t type_of_camera() override{
      return CAMERA_TYPE_FPS;
    }


};
