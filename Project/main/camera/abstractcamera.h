#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../globals.h"



using namespace glm;

class AbstractCamera {

private:

    mat4 view_matrix;

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

protected:

    vec3 up;
    vec3 position;
    vec3 center;


public:

    virtual ~AbstractCamera() {}

    virtual void Init(vec3 initEye = vec3(0), vec3 initCenter = vec3(0), vec3 up = vec3(0,1,0)) {
        this->position = initEye;
        global_position = initEye;
        this->up = up;
        this->center = initCenter;
        view_matrix = LookAt(position, center, up);
        global_view_matrix = LookAt(position, center, up);
    }

    virtual void Init(mat4 &view_matrix){
        this->view_matrix = view_matrix;
        global_view_matrix = view_matrix;
    }

    virtual void animate() = 0;

    virtual void rotate(float x, float z) = 0;

    virtual void move(float x, float y, float z) = 0;

    virtual void increaseVelocity() = 0;
    virtual void decreaseVelocity() = 0;

    virtual void beginFwAccel() = 0;
    virtual void beginBwAccel() = 0;
    virtual void beginYawAccel() = 0;
    virtual void beginReverseYawAccel() = 0;
    virtual void beginPitchAccel() = 0;
    virtual void beginReversePitchAccel() = 0;
    //virtual void beginRollAccel() = 0;

    virtual small_t type_of_camera() = 0;

    virtual bool switch_from_camera(mat4& view_matrix) = 0;

    mat4 getView() {
        return view_matrix;
    }
};
