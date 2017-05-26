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
    MultiTiles& multitiles;


public:

    Camera(MultiTiles& multitiles) : multitiles(multitiles) {

    }

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

        vec3 zpivot = normalize(center - position);
        vec3 xpivot = normalize(cross(up, zpivot));

        vec3 oldPosition = position;

        position += zpivot * z + xpivot * x + up*y;
        center += xpivot * x + zpivot * z + up*y;

            AbstractCamera::Init(position, center, up);

    }

    virtual void increaseVelocity() override {}
    virtual void decreaseVelocity() override {}

    virtual void beginFwAccel() override {}
    virtual void beginBwAccel() override{}

    virtual void beginYawAccel() override{}
    virtual void beginReverseYawAccel() override{}

    virtual void beginPitchAccel() override{}
    virtual void beginReversePitchAccel() override{}


};
