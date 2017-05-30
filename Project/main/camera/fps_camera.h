#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../terrain/terrain.h"
#include "../globals.h"
#include "../constants.h"

using namespace glm;

class fps_camera : public AbstractCamera {

private:

    const float MIN_DISTANCE_POLE = 0.1f;

    Terrain& terrain;
    MultiTiles& multitiles;


    mat4 LookAt(vec3 eye, vec3 center, vec3 up) {
        vec3 z_cam = normalize(eye - center);
        vec3 x_cam = normalize(cross(up, z_cam));
        vec3 y_cam = cross(z_cam, x_cam);

        mat3 R(x_cam, y_cam, z_cam);
        R = transpose(R);

        mat4 look_at(vec4(R[0], 0.0f),
                     vec4(R[1], 0.0f),
                     vec4(R[2], 0.0f),
                     vec4(-R * (eye), 1.0f));
        return look_at;
    }

    void update_fps_cam() {

      //if WASD was pressed before the current frame
      if (wasd_direction[0] != WASD_NULL || wasd_direction[1] != WASD_NULL) {

        size_t factors;
        float angle_sin = sin(global_angle_x) * INCREMENT_STEPS;
        float angle_cos = cos(global_angle_x) * INCREMENT_STEPS;

        bool cos_pos = angle_cos > 0.0f;
        //angle_cos*=angle_cos;

        if (angle_sin <=0.0f) {

          angle_sin*=angle_sin;

          if (wasd_direction[1] == WASD_D) {
            multitiles.decrementY(angle_sin);
          }
          else if (wasd_direction[1] == WASD_A) {
            multitiles.incrementY(angle_sin);
          }

          if (wasd_direction[0] == WASD_W) {
            multitiles.incrementX(angle_sin);
          }
          else if (wasd_direction[0] == WASD_S) {
            multitiles.decrementX(angle_sin);
          }
        }

        else {
          angle_sin*=angle_sin;

          if (wasd_direction[1] == WASD_D) {
            multitiles.incrementY(angle_sin);
          }
          else if (wasd_direction[1] == WASD_A) {
            multitiles.decrementY(angle_sin);
          }


          if (wasd_direction[0] == WASD_W) {
            multitiles.decrementX(angle_sin);
          }
          else if (wasd_direction[0] == WASD_S) {
            multitiles.incrementX(angle_sin);
          }

        }

        if (angle_cos <=0.0f) {
          angle_cos*=angle_cos;

          if (wasd_direction[1] == WASD_D) {
            multitiles.incrementX(angle_cos);
          }
          else if (wasd_direction[1] == WASD_A) {
            multitiles.decrementX(angle_cos);
          }


          if (wasd_direction[0] == WASD_W) {
            multitiles.incrementY(angle_cos);
          }
          else if (wasd_direction[0] == WASD_S) {
            multitiles.decrementY(angle_cos);
          }
        }


        else {
          angle_cos*=angle_cos;

          if (wasd_direction[1] == WASD_D) {
            multitiles.decrementX(angle_cos);
          }
          else if (wasd_direction[1] == WASD_A) {
            multitiles.incrementX(angle_cos);
          }

          if (wasd_direction[0] == WASD_W) {
            multitiles.decrementY(angle_cos);
          }
          else if (wasd_direction[0] == WASD_S) {
            multitiles.incrementY(angle_cos);
          }
        }


      }
    }




public:

    fps_camera(Terrain& terrain, MultiTiles& multitiles) : AbstractCamera(), terrain(terrain), multitiles(multitiles){}

    virtual ~fps_camera() {}




    virtual void animate() override {

      update_fps_cam();


      global_position = glm::vec3(quad_model_matrix * glm::vec4(1.0));
      position = glm::vec3(quad_model_matrix * glm::vec4(1.0));

      //global_position = glm::vec3(glm::vec4(position, 1.0f) * glm::translate(IDENTITY_MATRIX, glm::vec3(10,10,10)));
      //position = glm::vec3(glm::vec4(position, 1.0f) * glm::translate(IDENTITY_MATRIX, glm::vec3(10,10,10)));

      position[0] -= 5.0;
      position[2] -= 5.0;



        global_position.y = terrain.getCurrentHeight()*3 + 0.07;
        position.y = terrain.getCurrentHeight()*3 + 0.07;




      AbstractCamera::Init(position, center, up);
    }

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

        global_angle_x += x;

        AbstractCamera::Init(position, center, up);

    }

    virtual void move(float x, float y, float z) override {}

    virtual void increaseVelocity() override{}
    virtual void decreaseVelocity() override{}

    virtual void beginFwAccel() override{}
    virtual void beginBwAccel() override{}
    virtual void beginYawAccel() override{}
    virtual void beginReverseYawAccel() override{}
    virtual void beginPitchAccel() override{}
    virtual void beginReversePitchAccel() override{}
    //virtual void beginRollAccel() = 0;


};
