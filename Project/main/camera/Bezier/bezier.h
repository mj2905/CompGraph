#pragma once
#include <vector>
#include "icg_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

/*
vec3 operator*(const vec3& a, double b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}
*/

class Bezier {

private:
    const vector<vec3> points;

    vec3 apply(const vector<vec3>& points, float t) {
        assert(points.size() >= 1);
        if(points.size() == 1) {
            return points[0];
        }
        else {
            vector<vec3> newPoints(points.size()-1);
            for(size_t i = 0; i < newPoints.size(); ++i) {
                newPoints[i] = (1-t) * points[i] + t * points[i+1];
            }
            return apply(newPoints, t);
        }
    }


public:

    Bezier(vector<vec3>& points) : points(points) {
        assert(this->points.size() >= 2);
    }

    vec3 apply(float t) {
        assert(t >= 0 and t <= 1);
        return apply(points, t);
    }

};
