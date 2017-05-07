#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Bezier/bezier.h"
#include "abstractcamera.h"


using namespace glm;

class BezierCamera : public AbstractCamera {

private:

    Bezier camera_path;
    Bezier camera_aim;
    float time;

    const float MIN_DISTANCE_POLE = 0.1f;


public:

    BezierCamera(vector<vec3> camera_points, vector<vec3> aim_points) : AbstractCamera(), camera_path(camera_points), camera_aim(aim_points), time(0) {}

    virtual ~BezierCamera() {}

    virtual void Init(vec3 initEye = vec3(0), vec3 initCenter = vec3(0), vec3 up = vec3(0,1,0)) override {
        AbstractCamera::Init(camera_path.apply(time), camera_aim.apply(time), up);
    }


    virtual void animate() override {
        time += time + 0.001 > 1 ? 0 : 0.001;
        AbstractCamera::Init(camera_path.apply(time), camera_aim.apply(time), up);
    }

    void rotate(float x, float z) override {}

    void move(float x, float z) override {}


};
