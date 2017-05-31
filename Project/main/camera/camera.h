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

class Camera : public AbstractCamera {

private:

    const float MIN_DISTANCE_POLE = 0.1f;
    const float increment = 0.05;


public:

    virtual ~Camera() {}

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

        AbstractCamera::Init(position, center, up);

    }

    virtual void move(float x, float y, float z) override {

        x *= increment;
        y *= increment;
        z *= increment;

        vec3 zpivot = normalize(center - position);
        vec3 xpivot = normalize(cross(up, zpivot));

        vec3 oldPosition = position;

        position += zpivot * z + xpivot * x + up*y;
        center += xpivot * x + zpivot * z + up*y;

        //cout << "position " << position.x << " " << position.y << " " << position.z << endl;
        //cout << "center " << center.x << " " << center.y << " " << center.z << endl;

        AbstractCamera::Init(position, center, up);

    }

    virtual small_t type_of_camera() override {
      return CAMERA_TYPE_NORMAL ;
    }

    virtual void increaseVelocity() override {}
    virtual void decreaseVelocity() override {}

    virtual void beginFwAccel() override {}
    virtual void beginBwAccel() override{}

    virtual void beginYawAccel() override{}
    virtual void beginReverseYawAccel() override{}

    virtual void beginPitchAccel() override{}
    virtual void beginReversePitchAccel() override{}

    virtual void update_height() override {}


};
