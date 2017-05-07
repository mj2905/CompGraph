#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "abstractcamera.h"


using namespace glm;

class Camera : public AbstractCamera {

private:

    const float MIN_DISTANCE_POLE = 0.1f;


public:

    virtual ~Camera() {}

    virtual void animate() override {}

    void rotate(float x, float z) override {

        vec3 xpivot = normalize(cross(up, center - position));
        vec3 ypivot = normalize(cross(xpivot, center-position));
        vec3 oldCenter = center;
        center = (mat3(glm::rotate(glm::rotate(IDENTITY_MATRIX, x, ypivot), z, xpivot)) * (center - position)) + position;

        if(fmod(angle(normalize(center - position), up), M_PI - MIN_DISTANCE_POLE) < MIN_DISTANCE_POLE) {
            center = oldCenter;
        }
        else {
            AbstractCamera::Init(position, center, up);
        }

    }

    virtual void move(float x, float z) override {

        vec3 zpivot = normalize(center - position);
        vec3 xpivot = normalize(cross(up, zpivot));

        position += zpivot * z;
        if(x != 0) {
            position += xpivot * x;
            center += xpivot * x;
        }
        AbstractCamera::Init(position, center, up);
        cout << position.x << " " << position.y << " " << position.z << " " << center.x << " " << center.y << " " << center.z << endl;
    }

    virtual void increaseVelocity() override {}
    virtual void decreaseVelocity() override {}


};
